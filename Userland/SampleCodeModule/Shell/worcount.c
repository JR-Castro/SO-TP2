// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/wordcount.h"

void wordcount(int argc, char **argv) {
    uint64_t lines = 0;
    int c = getchar();
    while (c != EOF) {
        if (c == '\n') lines++;
        c = getchar();
    }
    char buff[30];
    itoa(lines, buff);
    fputs("Lines: ", STDOUT);
    puts(buff);
}
