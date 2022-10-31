// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/philosophers.h"

#define BLOCK 10
#define STARTPHILS 2

static int philoStatus[100];
static uint64_t philoPid[100];
static char *semaphores[100];
static int lock = 0;
static int size = 0;
static int philos = 0;


void printphilostatus() {
    for (int i = 0; i < philos; ++i) {
        if (philoStatus[i] == 0)
            putchar('.');
        else
            putchar('E');
    }
    putchar('\n');
}

// argv = { name, philoId }
_Noreturn static int philosopher(int argc, char **argv) {
    int id = satoi(argv[1]);
    int left = id;
    int right = (id + 1) % philos;

    sys_sem_open(semaphores[right], 0, 1);
    sys_sem_open(semaphores[left], 0, 1);

    while (1) {
        if (id % 2 == 0) {
            sys_sem_wait(semaphores[right]);
            sys_sem_wait(semaphores[left]);
        } else {
            sys_sem_wait(semaphores[left]);
            sys_sem_wait(semaphores[right]);
        }

        acquire(&lock);
        philoStatus[id] = 1;
        printphilostatus();
        release(&lock);

        bussy_wait(10000000);

        acquire(&lock);
        philoStatus[id] = 0;
        printphilostatus();
        release(&lock);

        sys_sem_post(semaphores[left]);
        sys_sem_post(semaphores[right]);
    }
}

void growArrays() {
//    char **newSemaphores = sys_alloc(sizeof(char *) * (size + BLOCK));
//    uint64_t *newPids = sys_alloc(sizeof(uint64_t) * (size + BLOCK));
//    int *newStatus = sys_alloc(sizeof(int *) * (size + BLOCK));
//    size += BLOCK;
//    for (int i = 0; i < philos; ++i) {
//        newSemaphores[i] = semaphores[i];
//        newPids[i] = philoPid[i];
//        newStatus[i] = philoStatus[i];
//    }
//    sys_free(semaphores);
//    sys_free(philoPid);
//    sys_free(philoStatus);
//    semaphores = newSemaphores;
//    philoPid = newPids;
//    philoStatus = newStatus;
}

void addphilo() {
    char *p_argv[2] = {"philo"};
    char buffer[30];
    char semname[30];
    strcpy(semname, "philo");
//    if (philos == size - 1)
//        growArrays();
    if (philos == 100)
        return;
    itoa(philos, buffer);
    strcat(semname, buffer);
    char *aux = sys_alloc(sizeof(char) * (strlen(semname) + 1));
    strcpy(aux, semname);
    semaphores[philos] = aux;
    p_argv[1] = buffer;
    sys_sem_open(semaphores[philos], 0, 1);
    philoStatus[philos] = 0;
    philos = philos + 1;
    char philoString[30];
    itoa(philos, philoString);
    puts(philoString);
    if ((philoPid[philos - 1] = sys_createProcess((void (*)(int, char **)) philosopher, 2, p_argv)) == 0) {
        fputs("Philosopher creation failed\n", STDERR);
        sys_free(semaphores[philos - 1]);
        philos = philos - 1;
    }
    sys_block(philoPid[philos - 1]);
    sys_free(aux);
}

void removephilo() {
    if (philos <= 2) {
        return;
    }
    philos--;
    sys_kill(philoPid[philos]);
    sys_sem_close(semaphores[philos]);
    sys_free(semaphores[philos]);
}

static void safeexit();

void philosophers(int argc, char **argv) {
    acquire(&lock);
    size = BLOCK;
//    philoStatus = sys_alloc(sizeof(int) * BLOCK);
//    semaphores = sys_alloc(sizeof(char *) * BLOCK);
//    philoPid = sys_alloc(sizeof(uint64_t) * BLOCK);


    for (int i = 0; i < STARTPHILS; i++) {
        addphilo();
    }

    for (int i = 0; i < STARTPHILS; ++i) {
        sys_unblock(philoPid[i]);
    }
    release(&lock);

    char c;
    while (1) {
        sys_read(STDIN, &c, 1);
        if (c == 'a') {
            acquire(&lock);
            addphilo();
            sys_unblock(philoPid[philos - 1]);
            release(&lock);
            continue;
        }
        if (c == 'r') {
            acquire(&lock);
            removephilo();
            release(&lock);
            continue;
        }
        if (c == 'e') {
            goto finish;
        }
    }
    finish:
    for (int i = 0; i < philos; ++i) {
        removephilo();
    }
    safeexit();
    return;
}

static void safeexit() {
    for (int i = 0; i < philos; ++i) {
        sys_kill(philoPid[i]);
    }
//    if (philoStatus) {
//        sys_free(philoStatus);
//    }
//    if (semaphores) {
//        for (int i = 0; i < philos; ++i) {
//            sys_sem_close(semaphores[i]);
//            sys_free(semaphores[i]);
//        }
//        sys_free(semaphores);
//    }
}
