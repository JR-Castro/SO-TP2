// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SO_TP2_STRINGLIB_H
#define SO_TP2_STRINGLIB_H

#include "defs.h"

char * strtok(char * str, const char * delim);

int strcmp(const char *s1, const char *s2);

int strlen(const char *s);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *src, size_t n);

char *strcat(char *dest, const char *src);

char *strncat(char *dest, const char *src, size_t n);

#endif //SO_TP2_STRINGLIB_H
