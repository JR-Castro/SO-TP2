// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SO_TP2_DEFS_H
#define SO_TP2_DEFS_H

#include <stdint.h>
#include <stddef.h>

#ifndef NULL
#define NULL ((void*)0)
#endif //NULL

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define EOF 4

//typedef unsigned long size_t;

typedef struct {
    uint8_t day, month, year;
    uint8_t hours, minutes, seconds;
} kernelTime_t;

struct memoryInfo {
    size_t free;
    size_t occupied;
    size_t totalSize;
};

typedef struct sem {
    char *name;
    uint64_t id, value, waiting;
    int lock;
} sem_t;

#endif //SO_TP2_DEFS_H
