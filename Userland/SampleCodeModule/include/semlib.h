// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com#ifndef SO_TP2_SEM_H
#ifndef SEMLIB_H
#define SEMLIB_H

#include "syslib.h"

extern int _xadd(int inc, int *value);
extern int _xchg(int *lock, int value);

typedef struct sem {
	char *name;
	uint64_t id;
	int lock;
	int value;
	int waiting;
} sem_t;

sem_t *sem_open(char *name, uint64_t id, int startValue);

void sem_wait(sem_t *sem);

void sem_post(sem_t *sem);

void sem_close(sem_t *sem);

#endif //SEMLIB_H
