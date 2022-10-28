// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// Implementation based on https://github.com/mit-pdos/xv6-public/blob/master/pipe.c

#include "include/pipe.h"

#define PIPESIZE 512

struct pipe {
    int lock;
    char data[PIPESIZE];
    char *name;
    uint64_t nread;
    uint64_t nwrite;
    int readopen;
    int writeopen;
    uint64List_t readers;
    uint64List_t writers;
};

uint64List_t pipeList;

static void wakeup(uint64List_t *list);

static void sleep(uint64List_t *list, int *lock);

static freepipe(struct pipe *p);

static void *getNamedPipe(char *name);

void *pipealloc() {
    struct pipe *p = (struct pipe *) memAlloc(sizeof(struct pipe));
    if (p == NULL)
        return NULL;
    p->lock = 0;
    p->nread = 0;
    p->nwrite = 0;
    p->readopen = 1;
    p->writeopen = 1;
    p->name = NULL;
    p->readers.first = p->readers.last = p->writers.last = p->writers.first = NULL;
    if (uint64ListAddNode(&(pipeList), (uint64_t)p)){
        memFree(p);
        return NULL;
    }
    return p;
}

void *namedPipeAlloc(char *name) {
    struct pipe *p = (struct pipe*) pipealloc();
    if (p == NULL)
        return NULL;

    int n = strlen(name) + 1
    p->name = memAlloc(sizeof(char) * n);
    if (p->name == NULL){
        freepipe(p);
        return NULL;
    }

    memcpy(p->name, name, n);
    return p;
}

void *connectNamedPipe(char *name) {
    struct pipe *p = getNamedPipe(name);
    return p == NULL ? namedPipeAlloc(name) : p;
}

void *getNamedPipe(char *name) {
    uint64Node_t node = pipeList.first;
    while (node != NULL) {
        if (strCmp(name, ((struct pipe*)node.val)->name) == 0)
            return (void*)node.val;
    }
    return NULL;
}

static freepipe(struct pipe *p) {
    uint64ListRemoveNode(&(pipeList), (uint64_t) p);
    uint64ListFree(&(p->readers));
    uint64ListFree(&(p->writers));
    if (p->name != NULL)
        memFree(p->name);
    memFree(p);
}

void pipeclose(struct pipe *p, int writable) {
    acquire(&(p->lock));
    if (writable) {
        p->writeopen--;
        wakeup(&(p->readers));
    } else {
        p->readopen--;
        wakeup(&(p->writers));
    }
    if (p->readopen == 0 && p->writeopen == 0) {
        freepipe(p);
    } else
        release(&(p->lock));
}

void addReader(struct pipe *p) {
    acquire(&(p->lock));
    p->readopen++;
    release(&(p->lock));
}

void addWriter(struct pipe *p) {
    acquire(&(p->lock));
    p->writeopen++;
    release(&(p->lock));
}

int pipewrite(struct pipe *p, char *addr, int n) {
    int i;

    acquire(&p->lock);
    for (i = 0; i < n; i++) {
        while (p->nwrite == p->nread + PIPESIZE) {  // full pipe
            if (p->readopen == 0 || killed(getPid())) {
                release(&(p->lock));
                return -1;
            }
            wakeup(&(p->readers));
            sleep(&(p->writers), &(p->lock));
        }
        p->data[p->nwrite++ % PIPESIZE] = addr[i];
    }
    wakeup(&(p->readers));
    release(&(p->lock));
    return n;
}

int piperead(struct pipe *p, char *addr, int n) {
    int i;

    acquire(&(p->lock));
    while (p->nread == p->nwrite && p->writeopen) {     // empty pipe
        if (killed(getPid())) {
            release(&p->lock);
            return -1;
        }
        sleep(&(p->readers), &(p->lock));
    }
    for (i = 0; i < n; i++) {
        if (p->nread == p->nwrite)
            break;
        addr[i] = p->data[p->nread++ % PIPESIZE];
    }
    wakeup(&(p->writers));
    release(&(p->lock));
    return i;
}

static void wakeup(uint64List_t *list) {
    uint64_t pid;
    while ((pid = uint64ListGetFirst(list)) != 0) {
        unblock(pid);
    }
}

static void sleep(uint64List_t *list, int *lock) {
    while (uint64ListAddNode(list, getPid())) {     // wait until there is enough memory
        yield();
        // TODO change so memory manager unblocks process when there is enough memory
    }
    release(lock);
    block(getPid());
    acquire(lock);
}
