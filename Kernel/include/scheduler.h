#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "naiveConsole.h"
#include "interrupts.h"
#include "timerDriver.h"
#include "defs.h"

uint64_t createProcess(void (*f)(int, char**), int argc, char **argv);

uint64_t schedule(uint64_t rsp);

uint64_t getPid();

void initializeScheduler();

uint64_t block(uint64_t pid);

uint64_t unblock(uint64_t pid);

uint64_t kill(uint64_t pid);

void yield();

uint64_t nice(uint64_t pid, uint64_t newPriority);

void forceTimerTick();

void printSchedulerInfo();

uint64_t waitPid(uint64_t pid);

/* Returns: 0 if process is not killed
 *          1 if process is killed or doesn't exist
 */
int killed(uint64_t pid);

int createPipe(int fd[2]);

int processConnectNamedPipe(char *name, int writable);

int dup2(int oldfd, int newfd);

int readFd(int fd, char *buffer, uint64_t n);

int writeFd(int fd, char *buffer, uint64_t count);

int closepipe(int fd);

#endif //SCHEDULER_H
