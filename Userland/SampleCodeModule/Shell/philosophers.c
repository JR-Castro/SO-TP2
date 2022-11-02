// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/philosophers.h"

#define LENGTH 30
#define MAXPHYLOS 20
#define STARTPHILS 5
#define PHYLOSEM "phylo_sem"
#define PHYLOWAIT 100000000

typedef enum {
    HUNGRY,
    EATING,
    THINKING,
} phyloState;

typedef struct {
    uint64_t pid;
    phyloState state;
    char semname[LENGTH];
} phylo_t;

static phylo_t phylos[MAXPHYLOS];
static int seated;

#define RIGHT(i) (((i)+1) % seated)
#define LEFT(i) (((i)+seated-1) % seated)

static void getForks(int index);
static void releaseForks(int index);
static void removephilo();
static void addphilo();
static void printPhylos();
static void test(int index);
static void removephilo();

// argv = { name, id }
_Noreturn static void philosopher(int argc, char **argv) {
    int index = satoi(argv[1]);
    while (1) {
        // change for randomized values
        getForks(index);
        bussy_wait(PHYLOWAIT);
        releaseForks(index);
        bussy_wait(PHYLOWAIT);
    }
}

static void addphilo() {
    if (seated >= MAXPHYLOS) {
        puts("No more space for philosophers");
        return;
    }
    sys_sem_wait(PHYLOSEM);

    char semname[LENGTH] = "phylo";
    char buffer[10];
    itoa(seated, buffer);
    strcat(semname, buffer);
    strcpy(phylos[seated].semname, semname);

    phylos[seated].state = THINKING;

    char argv1[LENGTH];
    itoa(seated, argv1);
    char *argv[] = {semname, argv1};

    if (sys_sem_open(phylos[seated].semname, 0, 1) == NULL) {
        fputs("Error opening semaphore\n", STDERR);
        return;
    }

    if ((phylos[seated].pid = sys_createProcess((int (*)(int, char **)) philosopher, 2, argv)) == 0) {
        fputs("Error creating process\n", STDERR);
        return;
    }
    seated++;
    sys_sem_post(PHYLOSEM);
}

static void removephilo() {
    if (seated == STARTPHILS)
        return;

    sys_sem_wait(PHYLOSEM);
    seated--;
    sys_sem_close(phylos[seated].semname);
    sys_kill(phylos[seated].pid);
    sys_sem_post(PHYLOSEM);
    return;
}

int philosophers(int argc, char **argv) {
    if (sys_sem_open(PHYLOSEM, 0, 1) == 0) {
        fputs("Error opening semaphore\n", STDERR);
        return -1;
    }
    seated = 0;
    for (int i = 0; i < STARTPHILS; i++) {
        addphilo(i);
    }

    char c;
    while (1) {
        sys_read(STDIN, &c, 1);
        switch (c) {
            case 'a':
                addphilo();
                break;
            case 'r':
                removephilo();
                break;
            case 'q':
                goto exit;
        }
    }

    exit:
    sys_sem_wait(PHYLOSEM);
    for (int i = 0; i < seated; ++i) {
        sys_sem_close(phylos[i].semname);
        sys_kill(phylos[i].pid);
    }
    sys_sem_close(PHYLOSEM);
    return 0;
}

static void getForks(int index) {
    sys_sem_wait(PHYLOSEM);
    phylos[index].state = HUNGRY;
    printPhylos();
    test(index);
    sys_sem_post(PHYLOSEM);
    sys_sem_wait(phylos[index].semname);
}

static void test(int index) {
    if (phylos[index].state == HUNGRY && phylos[LEFT(index)].state != EATING && phylos[RIGHT(index)].state != EATING){
        phylos[index].state = EATING;
        sys_sem_post(phylos[index].semname);
    }
}

static void releaseForks(int index) {
    sys_sem_wait(PHYLOSEM);
    phylos[index].state = THINKING;
    test(LEFT(index));
    test(RIGHT(index));
    printPhylos();
    sys_sem_post(PHYLOSEM);
}

static void printPhylos() {
    for (int i = 0; i < seated; ++i) {
        if (phylos[i].state == EATING)
            putchar('E');
        else
            putchar('.');
    }
    putchar('\n');
}

