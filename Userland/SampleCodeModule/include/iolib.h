// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SO_TP2_IOLIB_H
#define SO_TP2_IOLIB_H

#include "defs.h"
#include "stringlib.h"
#include "syslib.h"

int fputs(const char *str, int fd);

int puts(const char *str);

int putchar(char c);

int fgetchar(uint64_t fd);

int getchar();

char *gets(char *str);

char *fgets(char *str, int n, uint64_t fd);

#endif //SO_TP2_IOLIB_H
