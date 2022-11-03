// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SO_TP2_MEMMANAGERADT_H
#define SO_TP2_MEMMANAGERADT_H

#include "lib.h"

struct memoryInfo {
    size_t free;
    size_t occupied;
    size_t totalSize;
};

void initializeMemoryManager(void *start, uint64_t size);

void *memAlloc(size_t nbytes);

void memFree(void *ap);

void memoryInfo(struct memoryInfo *info);

#endif //SO_TP2_MEMMANAGERADT_H
