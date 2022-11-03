// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/filter.h"

int filter(int argc, char ** argv) {
    int c = getchar();
    while (c != EOF) {
        if (!charBelongs("aeiouAEIOU", c)) {
            putchar(c);
        }
        c = getchar();
    }
    return 0;
}
