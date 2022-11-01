// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/philosophers.h"

#define ARRAYSIZE 100
#define STARTPHILS 5

static int philoStatus[ARRAYSIZE];
static uint64_t philoPid[ARRAYSIZE];
static char *semaphores[ARRAYSIZE];
static int lock = 0;
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

// argv = { name, id, leftSem }
_Noreturn static int philosopher(int argc, char **argv) {
    int id = satoi(argv[1]);
    char *left = argv[1];

    acquire(&lock);
    char *right = semaphores[(id+1) % philos];
    release(&lock);

    sys_sem_open(left, 0, 1);
    sys_sem_open(right, 0, 1);

    while (1) {
        if (id % 2 == 0) {
            sys_sem_wait(right);
            sys_sem_wait(left);
        } else {
            sys_sem_wait(left);
            sys_sem_wait(right);
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

        sys_sem_post(left);
        sys_sem_post(right);

        acquire(&lock);
        if (strcmp(right, semaphores[(id+1)%philos]) != 0) {
            sys_sem_close(right);
            right = semaphores[(id+1)%philos];
            sys_sem_open(right, 0, 1);
        }
        release(&lock);
    }
}

void addphilo() {
    char *p_argv[3];
    char buffer[30];
    char left[30];

    if (philos >= ARRAYSIZE)
        return;

    strcpy(left, "phylo");

    itoa(philos, buffer);
    strcat(left, buffer);

    semaphores[philos] = sys_alloc(sizeof(char) * (strlen(left) + 1));
    strcpy(semaphores[philos], left);

    p_argv[0] = left;
    p_argv[1] = buffer;
    p_argv[2] = left;

    philoStatus[philos] = 0;
    philos = philos + 1;

    if ((philoPid[philos - 1] = sys_createProcess((void (*)(int, char **)) philosopher, 3, p_argv)) == 0) {
        fputs("Philosopher creation failed\n", STDERR);
        philos = philos - 1;
    }
}

void removephilo() {
    if (philos <= 2) {
        return;
    }
    philos--;
    sys_free(semaphores[philos]);
    sys_kill(philoPid[philos]);
}

static void safeexit();

void philosophers(int argc, char **argv) {
    acquire(&lock);

    for (int i = 0; i < STARTPHILS; i++) {
        addphilo();
    }
    release(&lock);

    char c;
    while (1) {
        sys_read(STDIN, &c, 1);
        if (c == 'a') {
            acquire(&lock);
            addphilo();
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
        sys_free(semaphores[i]);
    }
}
