// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/test_prio.h"

#define MINOR_WAIT 10000000
#define WAIT      100000000

#define TOTAL_PROCESSES 3
#define LOWEST 1
#define MEDIUM 5
#define HIGHEST 10

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

int test_prio(){
    int64_t pids[TOTAL_PROCESSES];
    char *argv[] = {"endless_loop_print"};
    uint64_t i;
    setEndlessLoopWait(MINOR_WAIT);

    for(i = 0; i < TOTAL_PROCESSES; i++)
        pids[i] = sys_createProcess((int (*)(int, char **)) endless_loop_print, 1, argv);

    bussy_wait(WAIT);
    puts("\nCHANGING PRIORITIES...");

    for(i = 0; i < TOTAL_PROCESSES; i++)
        sys_nice(pids[i], prio[i]);

    bussy_wait(WAIT);
    puts("\nBLOCKING...");

    for(i = 0; i < TOTAL_PROCESSES; i++)
        sys_block(pids[i]);

    puts("\nCHANGING PRIORITIES WHILE BLOCKED...");

    for(i = 0; i < TOTAL_PROCESSES; i++)
        sys_nice(pids[i], MEDIUM);

    puts("\nUNBLOCKING...");

    for(i = 0; i < TOTAL_PROCESSES; i++)
        sys_unblock(pids[i]);

    bussy_wait(WAIT);
    puts("\nKILLING...");

    for(i = 0; i < TOTAL_PROCESSES; i++)
        sys_kill(pids[i]);

    return 0;
}

