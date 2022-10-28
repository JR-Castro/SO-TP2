#ifndef PIPE_H

#include "lib.h"
#include "defs.h"
#include "semaphores.h"
#include "scheduler.h"
#include "memManager.h"

void *pipealloc();

int pipewrite(struct pipe *p, char *addr, int n);

int piperead(struct pipe *p, char *addr, int n);

void addReader(struct pipe *p);

void addWriter(struct pipe *p);

void pipeclose(struct pipe *p, int writable);

void *connectNamedPipe(char *name);

#endif // !PIPE_H
