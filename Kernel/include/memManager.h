//
// Created by monarch on 01/10/22.
//
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

void *mymalloc(size_t nbytes);

void myfree(void *ap);

void createMemoryManager(void *managedMemory, size_t size);

void memoryInfo(struct memoryInfo *info);


#endif //SO_TP2_MEMMANAGER_H
#endif //BUDDY