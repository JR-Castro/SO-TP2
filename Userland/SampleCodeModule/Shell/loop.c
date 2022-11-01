// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/loop.h"

_Noreturn void loop() {
    uint64_t pid = sys_getpid();
    char pidStr[30];
    itoa(pid, pidStr);
    while (1) {
        fputs("Hello - ", STDOUT);
        puts(pidStr);
        bussy_wait(100000000);
    }
}
