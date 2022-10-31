#ifndef SYSLIB_H
#define SYSLIB_H

#include <stdint.h>
#include "defs.h"
#include "stringlib.h"
#include "iolib.h"
#include "test_util.h"

void excepDivZero();

void excepInvalidOpcode();

unsigned int charBelongs(char *s, char c);

int containsString(const char *p1, const char *p2);

void savePrintMemParams(char *s);

void getTime();

void printPrime();

void fibonacciNumbs();

void printmem();

void inforeg();

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);

void itoa(int n, char s[]);

void *lib_memset(void *s, int c, size_t n);

extern uint64_t sys_read(uint64_t fd, char *buffer, uint64_t count);

extern uint64_t sys_write(uint64_t fd, const char *buffer, uint64_t count);

extern uint64_t sys_getregs(uint64_t *registers);

extern uint64_t sys_createProcess(void (*pFunction)(int, char **), int argc, char **argv);

extern uint64_t sys_waitpid(uint64_t pid);

extern void sys_time(time_t *time);

extern void sys_copymem(uint64_t address, uint8_t *buffer, uint64_t length);

extern uint64_t sys_getpid();

extern uint64_t sys_block(uint64_t pid);

extern uint64_t sys_unblock(uint64_t pid);

extern uint64_t sys_kill(uint64_t pid);

extern uint64_t sys_yield();

extern uint64_t sys_nice(uint64_t pid, uint64_t newPriority);

extern void sys_printSchedulerInfo();

extern void *sys_alloc(uint64_t size);

extern void sys_free(void *ptr);

extern void sys_memInfo(struct memoryInfo *info);

/* Opens semaphore: first it searches by id, then by name. If none is found,
 * it makes a new one.
 * Returns:     Pointer to semaphore if found or successfully created
 *              NULL if memory allocation failed somewhere
 */
extern sem_t *sys_sem_open(char *sem_name, uint64_t id, uint64_t initialValue);

/* Waits until semaphore is posted.
 * Returns:     0 if wait was successful
 *              -1 if memory allocation failed*/
extern uint64_t sys_sem_wait(char *sem);

/* Wakes a process in the waiting list for this semaphore */
extern uint64_t sys_sem_post(char *sem);

/* Removes the process from the semaphore list, deletes it if it's not used anymore */
extern uint64_t sys_sem_close(char *sem);

extern uint64_t sys_create_pipe(uint64_t fd[2]);

extern int sys_connect_named_pipe(char *name, int writeable);

extern int sys_dup2(uint64_t oldfd, uint64_t newfd);

extern void sys_print_pipe_info();

extern uint64_t sys_close_pipe(uint64_t fd);

extern void div_zero();

extern void inv_opcode();

extern int _xchg(int *lock, int value);

void acquire(int *lock);

void release(int *lock);


#endif
