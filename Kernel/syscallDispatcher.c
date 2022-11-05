// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/syscallDispatcher.h"
#include "include/scheduler.h"
#include "include/memManagerADT.h"
#include "include/pipe.h"

static void sys_time(kernelTime_t *s);

static void sys_copymem(uint64_t address, uint8_t *buffer, uint64_t length);

uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rax, uint64_t *registers) {
    switch (rax) {
        case 0:
            return readFd((int) rdi, (char *) rsi, rdx);
        case 1:
            return writeFd((int) rdi, (char *) rsi, rdx);
        case 2:
            return (uint64_t) getRegisters((uint64_t *) rdi);
        case 3:
            return createProcess((void (*)(int, char **)) rdi, (int) rsi, (char **) rdx);
        case 4:
            return waitPid(rdi);
        case 5:
            sys_time((kernelTime_t *) rdi);
            break;
        case 6:
            sys_copymem(rdi, (uint8_t *) rsi, rdx);
            break;
        case 7:
            return getPid();
        case 8:
            return block(rdi);
        case 9:
            return unblock(rdi);
        case 10:
            return kill(rdi);
        case 11:
            yield();
        case 12:
            return nice(rdi, rsi);
        case 13:
            return (uint64_t) printSchedulerInfo();
        case 14:
            return (uint64_t) memAlloc((size_t) rdi);
        case 15:
            memFree((void *) rdi);
            break;
        case 16:
            memoryInfo((struct memoryInfo *) rdi);
            break;
        case 17:
            return (uint64_t) sem_open((char *) rdi, rsi);
        case 18:
            return sem_wait((char *) rdi);
        case 19:
            sem_post((char *) rdi);
            break;
        case 20:
            sem_close((char *) rdi);
            break;
        case 21:
            return createPipe((int *) rdi);
        case 22:
            return processConnectNamedPipe((char *) rdi, rsi);
        case 23:
            return dup2(rdi, rsi);
        case 24:
            printPipesInfo();
            break;
        case 25:
            return closepipe(rdi);
        case 26:
            exitProcess();
            break;
        case 27:
            return (uint64_t) getSemaphoresInfo();
        default:
            return -1;
    }
    return 0;
}

static void sys_time(kernelTime_t *s) {
    s->day = localDay();
    s->month = localMonth();
    s->year = localYear();
    s->hours = localHours();
    s->minutes = getMinutes();
    s->seconds = getSeconds();
}

static void sys_copymem(uint64_t address, uint8_t *buffer, uint64_t length) {
    memcpy((void *) buffer, (void *) address, length);
}
