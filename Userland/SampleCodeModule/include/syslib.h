#ifndef SYSLIB_H
#define SYSLIB_H

#include <stdint.h>

#define NULL (void*)0

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define EOF (-1)

typedef unsigned long size_t;

typedef struct {
    uint8_t day, month, year;
    uint8_t hours, minutes, seconds;
} time_t;

struct memoryInfo {
    size_t free;
    size_t occupied;
    size_t totalSize;
};

typedef struct sem {
    char *name;
    uint64_t id, value, waiting;
    int lock;
} sem_t;

unsigned int strlen(const char *s);

void excepDivZero();

void excepInvalidOpcode();

int puts(const char *s);

int putChar(char);

char getChar();

int gets(char *s);

unsigned int charBelongs(char *s, char c);

int containsString(const char *p1, const char *p2);

void savePrintMemParams(char *s);

int strcmp(const char *p1, const char *p2);

void getTime();

void printPrime();

void fibonacciNumbs();

void printmem();

void inforeg();

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);

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

extern void div_zero();

extern void inv_opcode();

extern int _xchg(int *lock, int value);


#endif
