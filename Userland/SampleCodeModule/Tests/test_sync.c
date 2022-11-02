// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/test_sync.h"

#define SEM_ID "synctestsem"
#define TOTAL_PAIR_PROCESSES 5
#define INCWAIT 10000

int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc){
    uint64_t aux = *p;
    uint64_t wait = INCWAIT;
    while (wait) {
        wait--;
    }
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]){   // {name, n, inc, use_sem}
    uint64_t n;
    int8_t inc;
    int8_t use_sem = 0;
    sem_t *sem;

    if (argc < 4) return -1;

    if ((n = satoi(argv[1])) <= 0) return -1;
    if ((inc = satoi(argv[2])) == 0) return -1;
    if ((use_sem = satoi(argv[3])) < 0) return -1;

    if (use_sem)
        if (!(sem = sys_sem_open(SEM_ID,3, 1))){
            puts("test_sync: ERROR opening semaphore");
            return -1;
        }

    uint64_t i;
    for (i = 0; i < n; i++){
        if (use_sem) sys_sem_wait(SEM_ID);
        slowInc(&global, inc);
        if (use_sem) sys_sem_post(SEM_ID);
    }

    if (use_sem) sys_sem_close(SEM_ID);

    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]){ //{name, n, use_sem}
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];
    int8_t usesem;

    if (argc < 3) return -1;

    if ((usesem = satoi(argv[3])) < 0) return -1;

    char * argvDec[] = {"processDec", argv[1], "-1", argv[2]};
    char *argvInc[] = {"processInc", argv[1], "1", argv[2]};

    global = 0;

    uint64_t i;
    for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
        pids[i] = sys_createProcess((int (*)(int, char **)) my_process_inc, 4, argvDec);
        pids[i + TOTAL_PAIR_PROCESSES] = sys_createProcess((int (*)(int, char **)) my_process_inc, 4, argvInc);
    }

    for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
        sys_waitpid(pids[i]);
        sys_waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    fputs("Final value: ", STDOUT);
    char buffer[32];
    uintToBase(global, buffer, 10);
    puts(buffer);

    return 0;
}