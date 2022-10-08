// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <scheduler.h>
#include <memManager.h>
#include <string.h>

#define MAXFD 5
#define QUANTUM 1
#define STACK_SIZE 4096 // 4Kb
#define DEFAULT_PRIORITY 1
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10
#define FIRST_PID 1
#define DEFAULT_FLAGS 0x202
#define CODE_SEGMENT 0x8
#define STACK_SEGMENT 0

struct registerStack {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

typedef enum {
    READY,
    BLOCKED,
    KILLED,
} State;

typedef struct PCB {
    uint64_t rsp;
    State state;
    uint8_t ppid;
    uint8_t pid;
    uint64_t fd[MAXFD];
    uint64_t priority;
    uint64_t remainingCPUTime;
    void *stackMem;
    char **argv;
    int argc;
} pcb_t;

typedef struct processNode {
    pcb_t info;
    struct processNode *next;
} node_t;

typedef struct processList {
    node_t *first;
    node_t *last;
    uint64_t size;
    uint64_t nReady;
} pList_t;

static uint64_t pidCount = FIRST_PID;
static uint8_t size;
static pList_t processList;
static node_t *currentProcess;
static node_t *noProcess;

static uint64_t getNewPid();

static void copyArguments(char **dest, int argc, char **argv);

static void addProcess(node_t *node);

static node_t *removeProcess();

static void initializeProcessStack(node_t *pNode, void (*f)(int, char **));

static void loaderFunction(int argc, char **argv, void (*f)(int, char **));

static node_t *searchNode(uint64_t pid);

static void exitProcess();

static void freeProcess(node_t *node);

static void noProcessFunction(int argc, char **argv);

static void setRemainingTime(node_t *node);

void initializeScheduler() {
    processList.first = NULL;
    processList.last = NULL;
    processList.nReady = 0;
    processList.size = 0;
    currentProcess = NULL;
    char name[] = "noProcess";
    createProcess(noProcessFunction, 1, (char **) &name);
    noProcess = removeProcess();
}

uint64_t createProcess(void (*f)(int, char **), int argc, char **argv) {
    uint64_t pid = getNewPid();

    // Get memory for new process stack
    node_t *processNode = memAlloc(sizeof(node_t));
    if (processNode == NULL)
        return 0;
    void *processStack = memAlloc(STACK_SIZE);
    if (processStack == NULL) {
        memFree(processNode);
        return 0;
    }
    processNode->info.pid = pid;
    processNode->info.ppid = (currentProcess == NULL ? 0 : currentProcess->info.pid);
    processNode->info.stackMem = processStack;
    processNode->info.rsp = (uint64_t) processStack + STACK_SIZE;
    processNode->info.argv = memAlloc(sizeof(char *) * argc);
    processNode->info.state = READY;
    processNode->info.priority = DEFAULT_PRIORITY;
    setRemainingTime(processNode);
    copyArguments(processNode->info.argv, argc, argv);
    initializeProcessStack(processNode, f);
    addProcess(processNode);

    return pid;
}

static void initializeProcessStack(node_t *pNode, void (*f)(int, char **)) {
    struct registerStack *stack = (struct registerStack *) pNode->info.rsp;
    stack->rsi = (uint64_t) pNode->info.argv;
    stack->rdi = (uint64_t) pNode->info.argc;
    stack->rdx = (uint64_t) f;
    stack->rip = (uint64_t) loaderFunction;
    stack->rsp = (uint64_t) pNode->info.rsp;
    stack->rflags = DEFAULT_FLAGS;
    stack->cs = CODE_SEGMENT;
    stack->ss = STACK_SEGMENT;
}

static node_t *getReadyNode() {
    node_t *ret = removeProcess();
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

static void addProcess(node_t *node) {
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

static node_t *removeProcess() {
    if (processList.size == 0)
        return NULL;

    node_t *ans = processList.first;
    if (processList.last == ans)
        processList.last = NULL;

    processList.first = processList.first->next;
    if (ans->info.state == READY)
        processList.nReady--;
    processList.size--;
    return ans;
}

static void noProcessFunction(int argc, char **argv) {
    while (1)
        _hlt();
}

static int changeState(uint64_t pid, State newState) {
    node_t *aux = searchNode(pid);
    if (aux == NULL || aux->info.state == KILLED)
        return -1;

    if (aux->info.state == newState)
        return 1;

    if (newState == READY && aux->info.state != READY)
        processList.nReady++;
    else if (aux->info.state == READY && newState != READY)
        processList.nReady--;

    return 0;
}

uint64_t block(uint64_t pid) {
    if (pid <= FIRST_PID)
        return -1;
    changeState(pid, BLOCKED);
}

uint64_t unblock(uint64_t pid) {
    if (pid <= FIRST_PID)
        return -1;
    changeState(pid, READY);
}

uint64_t kill(uint64_t pid) {
    if (pid <= FIRST_PID)
        return -1;
    changeState(pid, KILLED);
}

uint64_t yield() {
    currentProcess->info.remainingCPUTime = 0;
    forceTimerTick();
}

static node_t *searchNode(uint64_t pid) {
    node_t *aux = processList.first;
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

static void freeProcess(node_t *node) {
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
    node_t *node;
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
        } else if (processList.nReady <= 0){
            currentProcess = noProcess;
            setRemainingTime(currentProcess);
        }
    }
    currentProcess->info.remainingCPUTime--;
    return currentProcess->info.rsp;
}

static void setRemainingTime(node_t *node){
    node->info.remainingCPUTime = node->info.priority * QUANTUM;
}
