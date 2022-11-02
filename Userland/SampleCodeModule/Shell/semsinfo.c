// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/semsinfo.h"

int semsinfo(int argc, char **argv) {
    char *auxmem = sys_alloc(4096);
    if (auxmem == NULL) {
        fputs("Error: No memory available.\n", STDERR);
        return -1;
    }
    sys_get_sems(auxmem);
    puts(auxmem);
    sys_free(auxmem);
    return 0;
}
