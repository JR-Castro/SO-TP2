// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com#ifndef SO_TP2_SEMAPHORES_H
#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include "memManager.h"
#include "lib.h"
#include "scheduler.h"
#include "uint64ListADT.h"

/* Opens semaphore: first it searches by id, then by name. If none is found,
 * it makes a new one.
 * Returns:     Pointer to semaphore if found or successfully created
 *              NULL if memory allocation failed somewhere
 */
void *sem_open(const char *name, uint64_t id, uint64_t startValue);

/* Waits until semaphore is posted.
 * Returns:     0 if wait was successful
 *              -1 if memory allocation failed*/
int sem_wait(void *sem);

/* Wakes a process in the waiting list for this semaphore */
void sem_post(void *sem);

/* Removes the process from the semaphore list, deletes it if it's not used anymore */
void sem_close(void *sem);

//extern int _xadd(int inc, int *value);
extern int _xchg(int *lock, int value);

#endif //SEMAPHORES_H