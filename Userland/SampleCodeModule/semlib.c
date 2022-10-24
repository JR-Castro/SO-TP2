// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include/semlib.h"

static void acquire(int *lock) {
	while(_xchg(lock,1) != 0);
}

static void release(int *lock){
	_xchg(lock, 0);
}

sem_t *sem_open(char *name, uint64_t id, int startValue){
	return (sem_t *)sys_sem_open(name, id, startValue);
}

void sem_wait(sem_t *sem){
	acquire(&(sem->lock));

	while (sem->value <= 0) {
		release(&(sem->lock));
		sys_sem_wait((void*)sem);
		acquire(&(sem->lock));
	}

	sem->value--;

	release(&(sem->lock));
}

void sem_post(sem_t *sem){
	acquire(&(sem->lock));
	s->value++;
	if (s->waiting > 0)
		sys_sem_post((void*)sem);
	release(&(sem->lock));
}

void sem_close(sem_t *sem){
	sys_sem_close(sem)
}

