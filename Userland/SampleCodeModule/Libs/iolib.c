// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/iolib.h"

int fputs(const char *str, int fd) {
    return sys_write(fd, str, strlen(str));
}

int puts(const char *str) {
    fputs(str, STDOUT);
    return fputs("\n", STDOUT);
}

int putchar(char c) {
    return sys_write(STDOUT, &c, 1);
}

int fgetchar(uint64_t fd) {
    char c;
    return sys_read(fd, &c, 1) == 1 ? (unsigned char)c : EOF;
}

int getchar() {
    return fgetchar(STDIN);
}

char *gets(char *str) {
    int i = 0, c;
    while ((c = getchar()) != '\n' && c != EOF) {
        str[i++] = c;
    }
    str[i] = '\0';
    return str;
}

char *fgets(char *str, int n, uint64_t fd) {
    int i = 0, c;
    while ((c = fgetchar(fd)) != '\n' && c != EOF && i < n - 1) {
        str[i++] = c;
    }
    str[i] = '\0';
    return str;
}
