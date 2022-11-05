// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/pipes.h"

int pipes(int argc, char **argv) {
    char *s = sys_print_pipe_info();
    if (s == NULL) {
        fputs("Error with memory allocation\n", STDERR);
        return -1;
    }
    puts(s);
    sys_free(s);
    return 0;
}