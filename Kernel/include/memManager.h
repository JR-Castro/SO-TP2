// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef BUDDY
#ifndef SO_TP2_MEMMANAGER_H
#define SO_TP2_MEMMANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

struct memoryInfo {
    size_t free;
    size_t occupied;
    size_t totalSize;
};

void *memAlloc(size_t nbytes);

void memFree(void *ap);

void createMemoryManager(void *managedMemory, size_t size);

void memoryInfo(struct memoryInfo *info);


#endif //SO_TP2_MEMMANAGER_H
#endif //BUDDY