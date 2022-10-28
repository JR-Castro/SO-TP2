#ifndef PIPE_H

#include "lib.h"
#include "defs.h"
#include "semaphores.h"
#include "scheduler.h"
#include "memManager.h"

int pipealloc(int fd[2]);

int pipewrite(struct pipe *p, char *addr, int n);

int piperead(struct pipe *p, char *addr, int n);

#endif // !PIPE_H
