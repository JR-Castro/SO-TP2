// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/scheduler.h"
#include "include/memManagerADT.h"
#include "include/lib.h"
#include "include/pipe.h"
#include "include/syscallDispatcher.h"

#define MAXFD 10
#define QUANTUM 1
#define STACK_SIZE 4096 // 4Kb
#define DEFAULT_PRIORITY 1
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10
#define FIRST_PID 1
#define STARTFD 3

typedef enum {
    READY,
    BLOCKED,
    KILLED,
} State;

typedef struct fd {
    void *p;
    int writable;
} fd_t;

typedef struct PCB {
    uint64_t rsp;
    State state;
    uint64_t ppid;
    uint64_t pid;
    fd_t fd[MAXFD];
    uint64_t priority;
    uint64_t remainingCPUTime;
    void *stackMem;
    char **argv;
    int argc;
    uint64List_t waiting;
} pcb_t;

typedef struct processNode {
    pcb_t info;
    struct processNode *next;
} pidNode_t;

typedef struct processList {
    pidNode_t *first;
    pidNode_t *last;
    uint64_t size;
    uint64_t nReady;
} pList_t;

static uint64_t pidCount = FIRST_PID;
static pList_t processList;
static pidNode_t *currentProcess;
static pidNode_t *noProcess;

static uint64_t getNewPid();

static int copyArguments(char **dest, int argc, char **argv);

static void addProcess(pidNode_t *node);

static pidNode_t *removeProcess();

static void loaderFunction(int argc, char **argv, void (*f)(int, char **));

static pidNode_t *searchNode(uint64_t pid);

static void freeProcess(pidNode_t *node);

_Noreturn static void noProcessFunction(int argc, char **argv);

static void setRemainingTime(pidNode_t *node);

static void copyfd(fd_t *parent, fd_t *child);

extern uint64_t setupStack(uint64_t startStack, uint64_t loader, uint64_t argc, uint64_t argv, uint64_t rip);

void initializeScheduler() {
    processList.first = NULL;
    processList.last = NULL;
    processList.nReady = 0;
    processList.size = 0;
    currentProcess = NULL;
    char *name[] = {"noProcess"};
    createProcess(noProcessFunction, 1, (char **) &name);
    noProcess = removeProcess();
}

uint64_t createProcess(void (*f)(int, char **), int argc, char **argv) {
    uint64_t pid = getNewPid();

    // Get memory for new process stack
    pidNode_t *processNode = memAlloc(sizeof(pidNode_t));
    if (processNode == NULL)
        return 0;
    void *processStack = memAlloc(STACK_SIZE);
    if (processStack == NULL) {
        memFree(processNode);
        return 0;
    }
    processNode->info.pid = pid;
    if (currentProcess == NULL || currentProcess == noProcess) {
        processNode->info.ppid = 0;
        for (int i = 0; i < MAXFD; ++i) {
            processNode->info.fd[i].p = NULL;
            processNode->info.fd[i].writable = -1;
        }
    } else {
        processNode->info.ppid = currentProcess->info.pid;
        copyfd(currentProcess->info.fd, processNode->info.fd);
    }
    processNode->info.stackMem = processStack;
    processNode->info.waiting.first = processNode->info.waiting.last = NULL;
    processNode->info.argc = argc;
    processNode->info.argv = memAlloc(sizeof(char *) * argc);
    if (processNode->info.argv == NULL) {
        memFree(processNode->info.stackMem);
        memFree(processNode);
        return 0;
    }
    uint64_t stackStart = (uint64_t) processStack + STACK_SIZE - 1;
    // Align memory to 64 bits
    stackStart -= stackStart % 8;
    processNode->info.rsp = setupStack(stackStart, (uint64_t) loaderFunction, argc,
                                       (uint64_t) processNode->info.argv, (uint64_t) f);
    processNode->info.state = READY;
    processNode->info.priority = DEFAULT_PRIORITY;

    setRemainingTime(processNode);
    if (copyArguments(processNode->info.argv, argc, argv)) {    // memory allocation for arguments failed
        memFree(processNode->info.stackMem);
        memFree(processNode);
        return 0;
    }
    addProcess(processNode);

    if (currentProcess == NULL)
        currentProcess = processNode;

    return pid;
}

static pidNode_t *getReadyNode() {
    pidNode_t *ret = removeProcess();
    while (ret->info.state != READY) {
        if (ret->info.state == KILLED) {
            freeProcess(ret);
            processList.size--;
        } else
            addProcess(ret);
        ret = removeProcess();
    }
    return ret;
}

static int copyArguments(char **dest, int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        size_t strsize = strlen(argv[i]);
        dest[i] = memAlloc(strsize + 1);
        if (dest[i] == NULL) {
            for (int j = 0; j < i; ++j) {
                memFree(dest[j]);
            }
            return -1;
        }
        memcpy(dest[i], argv[i], strsize + 1);
    }
    return 0;
}

static uint64_t getNewPid() {
    return ++pidCount;
}

static void addProcess(pidNode_t *node) {
    if (node == NULL)
        return;
    if (processList.first == NULL)
        processList.first = node;
    else
        processList.last->next = node;
    processList.last = node;
    node->next = NULL;
    processList.size++;
    if (node->info.state == READY)
        processList.nReady++;
}

static pidNode_t *removeProcess() {
    if (processList.size == 0)
        return NULL;

    pidNode_t *ans = processList.first;
    if (processList.last == ans)
        processList.last = NULL;

    processList.first = processList.first->next;
    if (ans->info.state == READY)
        processList.nReady--;
    processList.size--;
    return ans;
}

_Noreturn static void noProcessFunction(int argc, char **argv) {
    while (1)
        _hlt();
}

static int changeState(uint64_t pid, State newState) {
    pidNode_t *aux = searchNode(pid);
    if (aux == NULL || aux->info.state == KILLED)
        return -1;

    if (aux->info.state == newState)
        return 1;

    if (aux->info.state == READY && newState != READY)
        processList.nReady--;
    else if (aux->info.state != READY && newState == READY)
        processList.nReady++;

    aux->info.state = newState;

    if (newState == KILLED) {
        pidNode_t *previous = processList.first;
        if (previous == aux) {    // first node in the list
            processList.first = aux->next;
            if (processList.last == aux)
                processList.last = NULL;
        } else {
            while (previous != NULL && previous->next != aux) {
                previous = previous->next;
            }
            /* PVS says that previous may be a null pointer
             * But if we're killing a process in the list, either it's the first node
             * or there is another node pointing to it*/
            previous->next = aux->next;
            if (processList.last == aux)
                processList.last = previous;
        }
        freeProcess(aux);
        processList.size--;
    }

    if (pid == currentProcess->info.pid && newState != READY)
        yield();

    return 0;
}

uint64_t block(uint64_t pid) {
    if (pid <= FIRST_PID)
        return -1;
    uint64_t ans = changeState(pid, BLOCKED);
    return ans;
}

uint64_t unblock(uint64_t pid) {
    if (pid <= FIRST_PID)
        return -1;
    return changeState(pid, READY);
}

uint64_t kill(uint64_t pid) {
    if (pid <= FIRST_PID)
        return -1;
    return changeState(pid, KILLED);
}

void yield() {
    currentProcess->info.remainingCPUTime = 0;
    forceTimerTick();
}

static pidNode_t *searchNode(uint64_t pid) {
    pidNode_t *aux = processList.first;
    while (aux != NULL) {
        if (aux->info.pid == pid)
            return aux;
        aux = aux->next;
    }
    return NULL;
}

void exitProcess() {
    kill(currentProcess->info.pid);
}

static void freeProcess(pidNode_t *node) {

    for (int i = 0; i < MAXFD; ++i) {   // Close all pipes
        if (node->info.fd[i].p != NULL) {
            pipeclose(node->info.fd[i].p, node->info.fd[i].writable);
        }
    }

    uint64_t waiter = 0;
    while ((waiter = uint64ListGetFirst(&(node->info.waiting))) != 0) {  // Unblock all processes waiting for finish
        unblock(waiter);
    }

    for (int i = 0; i < node->info.argc; ++i) {
        memFree(node->info.argv[i]);
    }
    memFree(node->info.argv);
    memFree(node->info.stackMem);
    memFree(node);
}

static void loaderFunction(int argc, char **argv, void (*f)(int, char **)) {
    f(argc, argv);
    syscallExit();
}

uint64_t getPid() {
    return currentProcess->info.pid;
}

uint64_t nice(uint64_t pid, uint64_t newPriority) {
    if (newPriority < MIN_PRIORITY || newPriority > MAX_PRIORITY)
        return -1;
    pidNode_t *node;
    if (currentProcess->info.pid == pid)
        node = currentProcess;
    else {
        node = searchNode(pid);
        if (node == NULL)
            return -1;
    }
    node->info.priority = newPriority;
    return 0;
}

uint64_t schedule(uint64_t rsp) {
    if (currentProcess == NULL) {
        if (processList.size == 0) {
            currentProcess = noProcess;
            setRemainingTime(currentProcess);
        } else {
            currentProcess = removeProcess();
            setRemainingTime(currentProcess);
            addProcess(currentProcess);
        }
    } else {
        currentProcess->info.rsp = rsp;
        if (currentProcess->info.remainingCPUTime <= 0 && processList.nReady > 0) {
            currentProcess = getReadyNode();
            setRemainingTime(currentProcess);
            addProcess(currentProcess);
        } else if (processList.nReady <= 0) {
            currentProcess = noProcess;
            setRemainingTime(currentProcess);
        }
    }
    currentProcess->info.remainingCPUTime--;
    return currentProcess->info.rsp;
}

static void setRemainingTime(pidNode_t *node) {
    node->info.remainingCPUTime = node->info.priority * QUANTUM;
}

uint64_t waitPid(uint64_t pid) {
    if (pid == currentProcess->info.pid)
        return -1;
    pidNode_t *aux = searchNode(pid);
    if (aux != NULL && aux->info.state != KILLED) {
        uint64ListAddNode(&(aux->info.waiting), getPid());
        block(getPid());
    }
    return pid;
}

int killed(uint64_t pid) {
    pidNode_t *aux = searchNode(pid);
    if (aux == NULL || aux->info.state == KILLED)
        return 1;
    return 0;
}

void printSchedulerInfo(char *s) {
    char *ans, auxarray;
    char buffer[64] = {'0'};
    char title[] = "Name PID PPID Priority Stack BP\n";
    s[0] = '\0';
    strcat(s, title);
    pidNode_t *aux = processList.first;
    while (aux != NULL) {
        strcat(s, aux->info.argv[0]);
        strcat(s, " ");
        uintToBase(aux->info.pid, buffer, 10);
        strcat(s, buffer);
        strcat(s, " ");
        uintToBase(aux->info.ppid, buffer, 10);
        strcat(s, buffer);
        strcat(s, " ");
        uintToBase(aux->info.priority, buffer, 10);
        strcat(s, buffer);
        strcat(s, " ");
        uintToBase(aux->info.rsp, buffer, 16);
        strcat(s, buffer);
        strcat(s, " ");
        uintToBase((uint64_t)aux->info.stackMem, buffer, 16);
        strcat(s, buffer);
        strcat(s, "\n");
        aux = aux->next;
    }
}

int createPipe(int fd[2]) {
    int flag = 0;
    void *p = pipealloc();
    if (p == NULL)
        return -1;
    for (int i = STARTFD; i < MAXFD - 1; ++i) {
        if (currentProcess->info.fd[i].p == NULL) {
            currentProcess->info.fd[i].p = p;
            currentProcess->info.fd[i].writable = 0;
            fd[0] = i;
            flag = 1;
            break;
        }
    }
    addReader(p);
    if (!flag) {
        pipeclose(p, 0);
        return -1;
    }
    for (int i = fd[0]; i < MAXFD; ++i) {
        if (currentProcess->info.fd[i].p == NULL) {
            currentProcess->info.fd[i].p = p;
            currentProcess->info.fd[i].writable = 1;
            fd[1] = i;
            flag = 1;
            break;
        } else
            flag = 0;
    }
    if (!flag) {
        currentProcess->info.fd[fd[0]].p = NULL;
        currentProcess->info.fd[fd[0]].writable = -1;
        pipeclose(p, 0);
        return -1;
    }
    addWriter(p);
    return 0;
}

int processConnectNamedPipe(char *name, int writable) {
    void *p = connectNamedPipe(name);
    int ret = 0;
    if (p == NULL)
        return -1;

    for (int i = STARTFD; i < MAXFD; ++i) {
        if (currentProcess->info.fd[i].p == NULL) {
            currentProcess->info.fd[i].p = p;
            currentProcess->info.fd[i].writable = writable;
            ret = i;
        }
    }
    if (ret) {
        if (writable)
            addWriter(p);
        else
            addReader(p);
        return ret;
    }
    pipeclose(p, writable);
    return -1;
}

int dup2(int oldfd, int newfd) {
    if (newfd < 0 || MAXFD <= newfd)
        return -1;
    if (oldfd < 0 || MAXFD <= oldfd)
        return -1;
    fd_t *toclose = &(currentProcess->info.fd[newfd]);
    if (toclose->p != NULL) {
        pipeclose(toclose->p, toclose->writable);
        toclose->writable = -1;
    }
    fd_t *tocopy = &(currentProcess->info.fd[oldfd]);
    toclose->p = tocopy->p;
    toclose->writable = tocopy->writable;
    return 0;
}

int writeFd(int fd, char *buffer, uint64_t count) {
    if (fd < 0 || MAXFD <= fd)
        return -1;

    fd_t *p = &(currentProcess->info.fd[fd]);
    int i = 0;
    if (p->p != NULL) {
        if (p->writable == 0)
            return -1;
        else
            return pipewrite(p->p, buffer, count);
    }
    if (fd == STDIN) {
        return -1;
    }
    if (fd == STDOUT) {
        while (i < count) {
            ncPrintChar(buffer[i]);
            i++;
        }
    } else if (fd == STDERR) {
        while (i < count) {
            ncPrintCharFormat(buffer[i], ERROR_FORMAT);
            i++;
        }
    }
    return i;
}

int readFd(int fd, char *buffer, uint64_t n) {
    if (fd < 0 || MAXFD <= fd)
        return -1;

    fd_t *p = &(currentProcess->info.fd[fd]);
    if (p->p != NULL) {
        if (p->writable != 0)
            return -1;
        else
            return piperead(p->p, buffer, n);
    }
    if (fd != STDIN)
        return -1;
    return readBuffer(buffer, n);
}

static void copyfd(fd_t *parent, fd_t *child) {
    for (int i = 0; i < MAXFD; ++i) {
        child[i].p = parent[i].p;
        child[i].writable = parent[i].writable;
        if (child[i].p != NULL) {
            if (child[i].writable)
                addWriter(child[i].p);
            else
                addReader(child[i].p);
        }
    }
}

int closepipe(int fd) {
    if (fd < 0 || MAXFD <= fd)
        return -1;
    fd_t *p = &(currentProcess->info.fd[fd]);
    if (p->p != NULL) {
        pipeclose(p->p, p->writable);
        p->p = NULL;
        p->writable = -1;
    }
    return 0;
}
