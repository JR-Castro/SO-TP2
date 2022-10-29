// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/test_sync.h"

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 10
#define N 100
#define USESEM 1
#define INC 1
#define INCWAIT 1000000

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

uint64_t my_process_inc(uint64_t argc, char *argv[]){
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

//    if (argc != 3) return -1;
//
//    if ((n = satoi(argv[0])) <= 0) return -1;
//    if ((inc = satoi(argv[1])) == 0) return -1;
//    if ((use_sem = satoi(argv[2])) < 0) return -1;
    n = N;
    use_sem = USESEM;
    inc = INC;
    sem_t *sem;

    if (use_sem)
        if (!(sem = sys_sem_open(SEM_ID,3, 1))){
            puts("test_sync: ERROR opening semaphore\n");
            return -1;
        }

    uint64_t i;
    for (i = 0; i < n; i++){
//        puts("wait inc\n");
        if (use_sem) sys_sem_wait(SEM_ID);
//        puts("Incrementando\n");
        slowInc(&global, inc);
        if (use_sem) sys_sem_post(SEM_ID);
//        puts("post inc\n");
    }

    if (use_sem) sys_sem_close(SEM_ID);

    return 0;
}

uint64_t my_process_dec(uint64_t argc, char *argv[]){
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

//    if (argc != 3) return -1;
//
//    if ((n = satoi(argv[0])) <= 0) return -1;
//    if ((inc = satoi(argv[1])) == 0) return -1;
//    if ((use_sem = satoi(argv[2])) < 0) return -1;
    n = N;
    use_sem = USESEM;
    inc = -INC;
    sem_t *sem;

    if (use_sem)
        if (!(sem = sys_sem_open(SEM_ID,3, 1))){
            puts("test_sync: ERROR opening semaphore\n");
            return -1;
        }

    uint64_t i;
    for (i = 0; i < n; i++){
//        puts("wait dec\n");
        if (use_sem) sys_sem_wait(SEM_ID);
//        puts("Decrementando\n");
        slowInc(&global, inc);
        if (use_sem) sys_sem_post(SEM_ID);
//        puts("post dec\n");
    }

    if (use_sem) sys_sem_close(SEM_ID);

    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]){ //{n, use_sem, 0}
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

//    if (argc != 2) return -1;
//
//    char * argvDec[] = {argv[0], "-1", argv[1], NULL};
//    char * argvInc[] = {argv[0], "1", argv[1], NULL};
    char *argvInc[] = {"processInc"};
    char *argvDec[] = {"processDec"};

    global = 0;

    uint64_t i;
    for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
        pids[i] = sys_createProcess((void (*)(int, char **)) my_process_dec, 1, argvDec);
        pids[i + TOTAL_PAIR_PROCESSES] = sys_createProcess((void (*)(int, char **)) my_process_inc, 1, argvInc);
    }

    for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
        sys_waitpid(pids[i]);
        sys_waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
    }

    puts("Final value: ");
    char buffer[32] = {'0'};
    uintToBase(global, buffer, 10);
    puts(buffer);
    putChar('\n');

    return 0;
}