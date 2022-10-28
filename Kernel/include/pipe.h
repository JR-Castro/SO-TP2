#ifndef PIPE_H

#include "lib.h"
#include "defs.h"
#include "semaphores.h"
#include "scheduler.h"
#include "memManager.h"

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

void *pipealloc();

int pipewrite(struct pipe *p, char *addr, int n);

int piperead(struct pipe *p, char *addr, int n);

void addReader(struct pipe *p);

void addWriter(struct pipe *p);

void pipeclose(struct pipe *p, int writable);

void *connectNamedPipe(char *name);

void printPipesInfo();

#endif // !PIPE_H
