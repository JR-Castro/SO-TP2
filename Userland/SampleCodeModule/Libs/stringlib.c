// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/stringlib.h"

static int is_delim(char c, const char *delim) {
    while (*delim != '\0') {
        if (c == *delim) {
            return 1;
        }
        delim++;
    }
    return 0;
}

char * strtok(char * str, const char * delim) {
    static char *backup;
    if (!str) str = backup;
    if (!str) return NULL;

    while (1) {
        if (is_delim(*str, delim)) {
            str++;
            continue;
        }
        if (*str == '\0') return NULL;
        break;
    }
    char * ret = str;
    while (1) {
        if (*str == '\0') {
            backup = str;
            return ret;
        }
        if (is_delim(*str, delim)) {
            *str = '\0';
            backup = str + 1;
            return ret;
        }
        str++;
    }
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int strlen(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return ret;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (*src && n) {
        *dest = *src;
        dest++;
        src++;
        n--;
    }
    *dest = '\0';
    return ret;
}

char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest) dest++;
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return ret;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (*dest) dest++;
    while (*src && n) {
        *dest = *src;
        dest++;
        src++;
        n--;
    }
    *dest = '\0';
    return ret;
}
