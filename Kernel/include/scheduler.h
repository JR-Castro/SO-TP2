#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <naiveConsole.h>
#include <interrupts.h>
#include <timerDriver.h>
#include <defs.h>

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

uint64_t setupStack(uint64_t startStack, uint64_t loader, uint64_t argc, uint64_t argv, uint64_t rip);

#endif //SCHEDULER_H
