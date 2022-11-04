// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com#ifndef SO_TP2_SEMAPHORES_H
#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include "memManagerADT.h"
#include "lib.h"
#include "scheduler.h"
#include "uint64ListADT.h"

typedef struct sem {
    char *name;
    uint64_t id, value, waiting;
    int lock;
    uint64List_t waitingList, usingList;
} sem_t;

/* Opens semaphore: first it searches by id, then by name. If none is found,
 * it makes a new one.
 * Returns:     Pointer to semaphore if found or successfully created
 *              NULL if memory allocation failed somewhere
 */
void *sem_open(const char *name, uint64_t startValue);

/* Waits until semaphore is posted.
 * Returns:     0 if wait was successful
 *              -1 if memory allocation failed*/
int sem_wait(char *sem);

/* Wakes a process in the waiting list for this semaphore */
void sem_post(char *sem);

/* Removes the process from the semaphore list, deletes it if it's not used anymore */
void sem_close(char *sem);

//extern int _xadd(int inc, int *value);
extern int _xchg(int *lock, int value);

char *getSemaphoresInfo();

// Functions for managing locks

void acquire(int *lock);

void release(int *lock);

#endif //SEMAPHORES_H
