// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef BUDDY

#include <memManager.h>

/*
 * This memory manager is based on the one written on "C Programming Language" page 163
 * TODO Free asumes that the pointer given is a pointer to a valid block, that may not always be the case
 */

union header {
    struct {
        union header *ptr;
        size_t size;
    } s;
    long align; /* Force alignment of blocks */
};
typedef union header Header;

//static Header base;
static Header *freep = NULL;
struct memoryInfo memInfo;

void *memAlloc(size_t nbytes) {
    if (nbytes > memInfo.free)
        return NULL;

    Header *p, *prevp;
    size_t nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    prevp = freep;

    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {      // big enough
            if (p->s.size == nunits) {    // exactly
                prevp->s.ptr = p->s.ptr;
            } else {                      // allocate tail end
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            memInfo.free -= nunits * sizeof(Header);
            freep = prevp;
            return (void *) (p + 1);
        }
        if (p == freep)                 // wrapped around free list
            return NULL;
    }
}

void memFree(void *ap) {
    Header *freedBlock, *p;
    freedBlock = (Header *) ap - 1;               // Points to block header
    memInfo.free += freedBlock->s.size * sizeof(Header);

    for (p = freep; !(freedBlock > p && freedBlock < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (freedBlock > p || freedBlock < p->s.ptr))
            break;                      // freed block at start or end of arena

    // if the freed block and the next block are perfectly aligned
    if (freedBlock + freedBlock->s.size == p->s.ptr) {  // join to upper nbr
        freedBlock->s.size += p->s.ptr->s.size;
        freedBlock->s.ptr = p->s.ptr->s.ptr;
    } else
        freedBlock->s.ptr = p->s.ptr;
    // if the freed block and the current block are perfectly aligned
    if (p + p->s.size == freedBlock) {           // join to upper nbr
        p->s.size += freedBlock->s.size;
        p->s.ptr = freedBlock->s.ptr;
    } else
        p->s.ptr = freedBlock;
    freep = p;
}

void createMemoryManager(void *managedMemory, size_t size) {
    memInfo.totalSize = memInfo.free = size;
    freep = managedMemory;
    freep->s.ptr = managedMemory;
    freep->s.size = size / sizeof(Header);
}

void memoryInfo(struct memoryInfo *info) {
    info->free = memInfo.free;
    info->totalSize = memInfo.totalSize;
    info->occupied = memInfo.totalSize - memInfo.free;
}

#endif //BUDDY
