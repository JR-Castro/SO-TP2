// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/ps.h"

int ps(int argc, char **argv) {
    char *auxmem = sys_alloc(4096);
    if (auxmem == NULL) {
        fputs("Error: No memory available.\n", STDERR);
        return -1;
    }
    sys_printSchedulerInfo(auxmem);
    puts(auxmem);
    sys_free(auxmem);
    return 0;
}
