// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/printmeminfo.h"

void printmeminfo() {
    struct memoryInfo info;
    sys_memInfo(&info);
    fputs("Total memory: ", STDOUT);
    char buffer[64];
    itoa(info.totalSize, buffer);
    puts(buffer);

    fputs("Free memory: ", STDOUT);
    itoa(info.free, buffer);
    puts(buffer);

    fputs("Used memory: ", STDOUT);
    itoa(info.occupied, buffer);
    puts(buffer);
}
