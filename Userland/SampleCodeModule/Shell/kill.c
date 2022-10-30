// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/kill.h"

void kill(int argc, char **argv) {
    if (argc < 2) {
        puts("Usage: kill <pid>");
        return;
    }
    while (argc > 1) {
        int pid = satoi(argv[argc - 1]);
        if (pid > 0) {
            sys_kill(pid);
        }
        argc--;
    }
}
