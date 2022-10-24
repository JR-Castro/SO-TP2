// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "./include/semaphores.h"

#define STARTID 0
#define STARTLOCK 1

typedef struct pidNode {
    uint64_t val;
    struct pidNode *next;
} pidNode_t;

typedef struct pidList {
    pidNode_t *first, *last;
} pidList_t;

typedef struct sem {
    char *name;
    uint64_t id;
    uint64_t lock, value, waiting;
    pidList_t waitingList, usingList;
} sem_t;

typedef struct semNode {
    sem_t *sem;
    struct semNode *next;
} semNode_t;

int idCounter = STARTID;
semNode_t *semList = NULL;

// Functions for managing semaphores
static sem_t *createSemaphore(const char *name, uint64_t startValue);

static void freeSemaphore(sem_t *sem);

// Functions for managing list of semaphores
static int semListAdd(sem_t *sem);

static int semListDelete(sem_t *sem);

static semNode_t *semListSearchById(uint64_t id);

static semNode_t *semListSearchByName(const char *name);

static void semNodeFree(semNode_t *node);

// Functions for managing list of pids
static int pidListAddProcess(pidList_t *list, uint64_t pid);

static pidNode_t *pidListSearch(pidList_t *list, uint64_t pid);

static int pidListRemoveProcess(pidList_t *list, uint64_t pid);

static uint64_t getFirstPid(pidList_t *list);

static void pidListFree(pidList_t *list);


// Functions for managing semaphores

void *sem_open(const char *name, uint64_t id, uint64_t startValue) {
    semNode_t *node;
    if (id <= STARTID || id > idCounter) {
        node = semListSearchById(id);
    }
    if (node != NULL) {
        if (pidListAddProcess(&(node->sem->usingList), getPid()))
            return NULL;
        return node;
    }

    node = semListSearchByName(name);
    if (node != NULL) {
        if (pidListAddProcess(&(node->sem->usingList), getPid()))
            return NULL;
        return node;
    }

    // semaphore doesn't exist, create it.
    return (void *) createSemaphore(name, startValue);
}

void sem_post(void *sem) {
    sem_t *s = (sem_t *) sem;
    uint64_t pid = getFirstPid(&(s->waitingList));
    // Retry until a process is unblocked
    while (pid != 0) {
        if (unblock(pid) == 0)
            break;
        pid = getFirstPid(&(s->waitingList));
    }
}

int sem_wait(void *sem) {
    sem_t *s = (sem_t *) sem;
    if (pidListAddProcess(&(s->waitingList), getPid()))
        return -1;
    s->waiting++;
    block(getPid());
    return 0;
}

void sem_close(void *sem) {
    sem_t *s = (sem_t *) sem;
    pidListRemoveProcess(&(s->usingList), getPid());
    pidListRemoveProcess(&(s->waitingList), getPid()); // Just in case
    if (s->usingList.first == NULL && s->waitingList.first == NULL) {
        semListDelete(s);
        freeSemaphore(s);
    }
}

static void freeSemaphore(sem_t *sem) {
    memFree(sem->name);
    pidListFree(&(sem->waitingList));
    pidListFree(&(sem->usingList));
}

/* Creates semaphore and setups all necessary auxiliar structures
 * Returns: pointer to semaphore if successful
 *          NULL, if memory allocation failed
 */
static sem_t *createSemaphore(const char *name, uint64_t startValue) {
    sem_t *sem = memAlloc(sizeof(sem_t));
    if (sem == NULL)
        return NULL;

    int length = strlen(name);
    char *semName = memAlloc(sizeof(char) * (length + 1));
    if (semName == NULL) {
        memFree(sem);
        return NULL;
    }
    memcpy(semName, name, length + 1);

    sem->lock = sem->waiting = 0;
    sem->id = ++idCounter;
    sem->value = startValue;
    sem->lock = STARTLOCK;
    sem->waitingList.first = sem->waitingList.last = NULL;
    sem->usingList.first = sem->usingList.last = NULL;

    if (pidListAddProcess(&(sem->usingList), getPid())) {
        freeSemaphore(sem);
        return NULL;
    }
    if (semListAdd(sem)) {
        freeSemaphore(sem);
        return NULL;
    }
    return sem;
}

// Functions for managing list of semaphores

/* Adds semaphore to semList
 * Returns: 0 if successfully added
 *          -1 if memory allocation for node failed
 */
static int semListAdd(sem_t *sem) {
    semNode_t *current = semList;
    semNode_t *newNode = (semNode_t *) memAlloc(sizeof(semNode_t));
    if (newNode == NULL)
        return -1;
    newNode->sem = sem;
    newNode->next = NULL;

    if (current == NULL) {
        semList = newNode;
        return 0;
    }
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
    return 0;
}

/* Deletes sem from list of semaphores
 * Returns: 0 if sucessfully deleted
 *          1 if sem didn't exist
 */
static int semListDelete(sem_t *sem) {
    semNode_t *previous = NULL, *current = semList;
    while (current != NULL) {
        if (current->sem == sem)
            break;
        previous = current;
        current = current->next;
    }
    if (current == NULL)
        return 1;   // Nothing to delete

    if (previous == NULL)
        semList = current->next;
    else
        previous->next = current;
    semNodeFree(current);
    return 0;
}

static void semNodeFree(semNode_t *node) {
    freeSemaphore(node->sem);
    memFree(node);
}

/* Searches semList for node with same id
 * Returns: pointer to node if found
 *          NULL otherwise
 */
static semNode_t *semListSearchById(uint64_t id) {
    semNode_t *current = semList;
    while (current != NULL) {
        if (current->sem->id == id)
            break;
        current = current->next;
    }
    return current;
}

/* Searches semList for node with matching name (case-sensitive)
 * Returns: pointer to node if found
 *          NULL otherwise
 */
static semNode_t *semListSearchByName(const char *name) {
    semNode_t *current = semList;
    while (current != NULL) {
        if (strCmp(current->sem->name, name) == 0)
            break;
        current = current->next;
    }
    return current;
}

// Functions for managing list of PIDs

/* Adds a node with the specified pid at the end of the list
 * Returns: 0 if successful
 *          -1 if memory allocation failed*/
static int pidListAddProcess(pidList_t *list, uint64_t pid) {
    pidNode_t *newNode = (pidNode_t *) memAlloc(sizeof(pidNode_t));
    if (newNode == NULL)
        return -1;

    newNode->val = pid;

    if (list->first == NULL)
        list->first = newNode;
    else
        list->last->next = newNode;
    list->last = newNode;
    return 0;
}

/* Searches specified pidList for node with matching pid
 * Returns: Pointer to node if found
 *          NULL otherwise*/
static pidNode_t *pidListSearch(pidList_t *list, uint64_t pid) {
    pidNode_t *current = list->first;
    while (current != NULL) {
        if (current->val == pid)
            break;
        current = current->next;
    }
    return current;
}

/* Deletes pid from list of PIDs
 * Returns: 0 if sucessfully deleted
 *          1 if PID wasn't found
 */
static int pidListRemoveProcess(pidList_t *list, uint64_t pid) {
    pidNode_t *previous = NULL, *current = list->first;
    while (current != NULL) {
        if (current->val == pid)
            break;
        previous = current;
        current = current->next;
    }
    if (current == NULL)
        return 1;   // Nothing to delete

    if (previous == NULL)
        list->first = current->next;
    else
        previous->next = current->next;
    memFree(current);
    return 0;
}

/* Returns: pid of first node if list is not empty
 *          0 if list is empty*/
static uint64_t getFirstPid(pidList_t *list) {
    if (list->first == NULL)
        return 0;
    uint64_t pid = list->first->val;
    pidNode_t *first = list->first;
    list->first = list->first->next;
    memFree(first);
    return pid;
}

static void pidListFree(pidList_t *list) {
    pidNode_t *current = list->first, *next;
    while (current != NULL) {
        next = current->next;
        memFree(current);
        current = next;
    }
}
