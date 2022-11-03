// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/memManagerADT.h"
#ifdef BUDDY
#define BUDDY_ALLOC_IMPLEMENTATION
#include "buddy.h"
#else
#include "include/memManager.h"
#endif

struct buddy *buddy = NULL;

void initializeMemoryManager(void *start, uint64_t size) {
    #ifdef BUDDY
    buddy = buddy_embed(start, size);
    #else
    dumbCreateMemoryManager(start, size);
    #endif
}

void *memAlloc(size_t nbytes) {
    #ifdef BUDDY
    return buddy_malloc(buddy, nbytes);
    #else
    return dumbManagerAlloc(nbytes);
    #endif
}

void memFree(void *ap) {
    #ifdef BUDDY
    buddy_free(buddy, ap);
    #else
    dumbManagerFree(ap);
    #endif
}

static void *walkFunction(void *ctx, void *addr, size_t slot_size) {
    size_t *occupied = (size_t *) ctx;
    *occupied += slot_size;
    return NULL;
}

void memoryInfo(struct memoryInfo *info) {
    #ifdef BUDDY
    info->occupied = 0;
    info->totalSize = buddy_arena_size(buddy);
    buddy_walk(buddy, walkFunction, &(info->occupied));
    info->free = info->totalSize - info->occupied;
    #else
    dumbMemoryInfo(info);
    #endif
}
