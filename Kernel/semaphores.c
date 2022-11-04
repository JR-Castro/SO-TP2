// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "./include/semaphores.h"

#define STARTID 0
#define STARTLOCK 0

typedef struct semNode {
    sem_t *sem;
    struct semNode *next;
} semNode_t;

int idCounter = STARTID;
semNode_t *semList = NULL;
int semListLock = STARTLOCK;

// Functions for managing semaphores
static sem_t *createSemaphore(const char *name, uint64_t startValue);

static void freeSemaphore(sem_t *sem);

// Functions for managing list of semaphores
static int semListAdd(sem_t *sem);

static int semListDelete(sem_t *sem);

static semNode_t *semListSearchByName(const char *name);

static void semNodeFree(semNode_t *node);


// Functions for managing semaphores

void *sem_open(const char *name, uint64_t startValue) {
    semNode_t *node = NULL;
    acquire(&semListLock);

    node = semListSearchByName(name);
    if (node != NULL) {
        if (uint64ListAddNode(&(node->sem->usingList), getPid())) {
            release(&(semListLock));
            return NULL;
        }
        release(&semListLock);
        return node;
    }

    // semaphore doesn't exist, create it.
    void *sem = createSemaphore(name, startValue);
    release(&semListLock);
    return sem;
}

void sem_post(char *sem) {
    acquire(&(semListLock));
    semNode_t *node = semListSearchByName(sem);
    if (node == NULL) {
        release(&(semListLock));
        return;
    }
    sem_t *s = node->sem;
    release(&(semListLock));
    acquire(&(s->lock));
    s->value++;
    if (s->waiting > 0) {
        uint64_t pid = uint64ListGetFirst(&(s->waitingList));
        // Retry until a process is unblocked
        while (pid != 0) {
            if (unblock(pid) == 0)
                break;
            else
                s->waiting--;
            pid = uint64ListGetFirst(&(s->waitingList));
        }
    }
    release(&(s->lock));
}

int sem_wait(char *sem) {
    acquire(&(semListLock));
    semNode_t *node = semListSearchByName(sem);
    if (node == NULL) {
        release(&(semListLock));
        return -1;
    }
    sem_t *s = node->sem;
    release(&(semListLock));
    acquire(&(s->lock));
    int first = 1;

    while (s->value <= 0) {
        if (first) {
            first = 0;
            s->waiting++;
        }
        if (uint64ListAddNode(&(s->waitingList), getPid())) {
            return -1;
        }
        release(&(s->lock));
        block(getPid());
        acquire(&(s->lock));
    }

    if (!first) {
        s->waiting--;
    }
    s->value--;
    release(&(s->lock));

    return 0;
}

void sem_close(char *sem) {
    acquire(&(semListLock));
    semNode_t *node = semListSearchByName(sem);
    if (node == NULL) {
        release(&(semListLock));
        return;
    }
    sem_t *s = node->sem;
    acquire(&(s->lock));
    uint64ListRemoveNode(&(s->usingList), getPid());
    uint64ListRemoveNode(&(s->waitingList), getPid()); // Just in case
    if (s->usingList.first == NULL && s->waitingList.first == NULL) {
        semListDelete(s);
        freeSemaphore(s);
    }
    release(&semListLock);
    release(&(s->lock));
}

static void freeSemaphore(sem_t *sem) {
    memFree(sem->name);
    uint64ListFree(&(sem->waitingList));
    uint64ListFree(&(sem->usingList));
    memFree(sem);
}

/* Creates semaphore and setups all necessary auxiliary structures
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
    sem->name = semName;
    sem->waitingList.first = sem->waitingList.last = NULL;
    sem->usingList.first = sem->usingList.last = NULL;

    if (uint64ListAddNode(&(sem->usingList), getPid())) {
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
 * Returns: 0 if successfully deleted
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
        previous->next = current->next;
    semNodeFree(current);
    return 0;
}

static void semNodeFree(semNode_t *node) {
    freeSemaphore(node->sem);
    memFree(node);
}

/* Searches semList for node with matching name (case-sensitive)
 * Returns: pointer to node if found
 *          NULL otherwise
 */
static semNode_t *semListSearchByName(const char *name) {
    semNode_t *current = semList;
    while (current != NULL) {
        if (strcmp(current->sem->name, name) == 0)
            break;
        current = current->next;
    }
    return current;
}

char *getSemaphoresInfo() {
    acquire(&semListLock);
    semNode_t *aux = semList;
    char title[] = "Name Id Value Waiting\n";
    char buffer[64] = {'0'};
    char *ans = NULL;
    int length = 0, size = 0;

    length = copyResizeableString(&ans, title, &size, length);
    if (length == -1) {
        release(&semListLock);
        return NULL;
    }

    while (aux != NULL) {
        length = copyResizeableString(&ans, aux->sem->name, &size, length);
        if (length == -1)
            goto bad;
        length = copyResizeableString(&ans, " ", &size, length);
        if (length == -1)
            goto bad;

        uintToBase(aux->sem->id, buffer, 10);
        length = copyResizeableString(&ans, buffer, &size, length);
        if (length == -1)
            goto bad;
        length = copyResizeableString(&ans, " ", &size, length);
        if (length == -1)
            goto bad;

        uintToBase(aux->sem->value, buffer, 10);
        length = copyResizeableString(&ans, buffer, &size, length);
        if (length == -1)
            goto bad;
        length = copyResizeableString(&ans, " ", &size, length);
        if (length == -1)
            goto bad;

        uintToBase(aux->sem->waiting, buffer, 10);
        length = copyResizeableString(&ans, buffer, &size, length);
        if (length == -1)
            goto bad;

        length = copyResizeableString(&ans, "\n", &size, length);
        if (length == -1)
            goto bad;
        aux = aux->next;
    }

    release(&semListLock);
    finishResizeableString(&ans, length);
    return ans;

    bad:
    release(&semListLock);
    memFree(ans);
    return NULL;
}

// Functions for managing locks

void acquire(int *lock) {
    while (_xchg(lock, 1) != 0);
}

void release(int *lock) {
    _xchg(lock, 0);
}
