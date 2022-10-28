// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include/pipe.h"

#define PIPESIZE 512

struct pipe {
    int lock;
    char data[PIPESIZE];
    uint64_t nread;
    uint64_t nwrite;
    int readopen;
    int writeopen;
    uint64List_t readers;
    uint64List_t writers;
};

static void wakeup(uint64List_t *list);

static void sleep(uint64List_t *list, int *lock);

int pipealloc(int fd[2]) {
    struct pipe *p = (struct pipe *) memAlloc(sizeof(struct pipe));
    if (p == NULL)
        return -1;
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
        if (unblock(pid) == 0)
            break;
    }
}

static void sleep(uint64List_t *list, int *lock) {
    uint64ListAddNode(list, getPid());
    release(lock);
    block(getPid());
    acquire(lock);
}
