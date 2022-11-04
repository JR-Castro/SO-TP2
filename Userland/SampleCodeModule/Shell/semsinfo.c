// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/semsinfo.h"

int semsinfo(int argc, char **argv) {
    char *ans = sys_get_sems();
    if (ans == NULL) {
        fputs("Error getting semaphores info\n", STDERR);
        return -1;
    }
    puts(ans);
    sys_free(ans);
    return 0;
}
