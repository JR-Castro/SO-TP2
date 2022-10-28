// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/scheduler.h"
#include "include/memManager.h"
#include "include/lib.h"
#include "include/pipe.h"
#include "include/syscallDispatcher.h"

#define MAXFD 5
#define QUANTUM 1
#define STACK_SIZE 4096 // 4Kb
#define DEFAULT_PRIORITY 1
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10
#define FIRST_PID 1
#define STARTFD 2

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

static void copyArguments(char **dest, int argc, char **argv);

static void addProcess(pidNode_t *node);

static pidNode_t *removeProcess();

static void loaderFunction(int argc, char **argv, void (*f)(int, char **));

static pidNode_t *searchNode(uint64_t pid);

static void exitProcess();

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
    if (currentProcess == NULL) {
        processNode->info.ppid = currentProcess->info.pid;
        for (int i = STARTFD; i < MAXFD; ++i) {
            processNode->info.fd[i].p = NULL;
            processNode->info.fd[i].writable = -1;
        }
    } else {
        processNode->info.ppid = currentProcess->info.pid;
        copyfd(currentProcess->info.fd, processNode->info.fd);
    }
    processNode->info.stackMem = processStack;
    processNode->info.argv = memAlloc(sizeof(char *) * argc);
    uint64_t stackStart = (uint64_t) processStack + STACK_SIZE - 1;
    // Align memory to 64 bits
    stackStart -= stackStart % 8;
    processNode->info.rsp = setupStack(stackStart, (uint64_t) loaderFunction, argc,
                                       (uint64_t) processNode->info.argv, (uint64_t) f);
    processNode->info.state = READY;
    processNode->info.priority = DEFAULT_PRIORITY;

    setRemainingTime(processNode);
    copyArguments(processNode->info.argv, argc, argv);
    addProcess(processNode);

    if (currentProcess == NULL)
        currentProcess = processNode;

    return pid;
}

static pidNode_t *getReadyNode() {
    pidNode_t *ret = removeProcess();
    while (ret->info.state != READY) {
        if (ret->info.state == KILLED)
            freeProcess(ret);
        else
            addProcess(ret);
        ret = removeProcess();
    }
    return ret;
}

static void copyArguments(char **dest, int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        size_t strsize = strlen(argv[i]);
        dest[i] = memAlloc(strsize + 1);
        if (argv[i] == NULL)
            return;
        memcpy(dest[i], argv[i], strsize + 1);
    }
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
        for (int i = 0; i < MAXFD; ++i) {
            if (aux->info.fd[i].p != NULL) {
                pipeclose(aux->info.fd[i].p, aux->info.fd[i].writable);
            }
        }
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

static void exitProcess() {
    kill(currentProcess->info.pid);
}

static void freeProcess(pidNode_t *node) {
    for (int i = 0; i <= node->info.argc; ++i) {
        memFree(node->info.argv[i]);
    }
    memFree(node->info.argv);
    memFree(node->info.stackMem);
    memFree(node);
}

static void loaderFunction(int argc, char **argv, void (*f)(int, char **)) {
    f(argc, argv);
    exitProcess();
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
    pidNode_t *aux;
    while ((aux = searchNode(pid)) != NULL && aux->info.state != KILLED) {
        yield();
    }
    return pid;
}

int killed(uint64_t pid) {
    pidNode_t *aux = searchNode(pid);
    if (aux == NULL || aux->info.state == KILLED)
        return 1;
    return 0;
}

void printSchedulerInfo() {
    pidNode_t *aux = currentProcess;
    ncNewline();
    ncPrint("Name PID PPID Priority Stack BP");
    while (aux != NULL) {
        ncPrint(aux->info.argv[0]);
        ncPrintChar(' ');
        ncPrintDec(aux->info.pid);
        ncPrintChar(' ');
        ncPrintDec(aux->info.ppid);
        ncPrintChar(' ');
        ncPrintDec(aux->info.priority);
        ncPrintChar(' ');
        ncPrintHex(aux->info.rsp);
        ncPrintChar(' ');
        ncPrintHex((uint64_t) aux->info.stackMem);
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
    if (newfd < 0 || MAXFD <= oldfd)
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
