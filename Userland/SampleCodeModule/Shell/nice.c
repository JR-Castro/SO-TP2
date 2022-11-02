// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/nice.h"

int nice(int argc, char **argv) {
    if (argc < 3) {
        puts("Usage: nice <pid> <priority>");
        return 0;
    }
    int pid = satoi(argv[1]);
    int priority = satoi(argv[2]);
    if (pid > 0 && priority >= 0 && priority <= 10) {
        sys_nice(pid, priority);
    }
    return 0;
}