// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef BUDDY

#include "include/memManager.h"

/*
 * This memory manager is based on the one written on "The C Programming Language" page 163
 */

#define BLOCK 512

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
static Header **blocks = NULL;
uint64_t blockArraySize = 0;
uint64_t occupied = 0;
struct memoryInfo memInfo;

static int growBlockArray();

static int isInBlockArray(Header *block);

static void deleteFromBlockArray(Header *block);

void *dumbManagerAlloc(size_t nbytes) {
    if (nbytes > memInfo.free)
        return NULL;

    Header *p, *prevp;
    size_t nunits;
    uint8_t exactsize = 0;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    prevp = freep;

    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {      // big enough
            if (p->s.size == nunits) {    // exactly
                prevp->s.ptr = p->s.ptr;
                exactsize = 1;
            } else {                      // allocate tail end
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            if (occupied == blockArraySize - 2) {
                if (growBlockArray()) {
                    /*  If we can't grow the array, we can't allocate memory
                     *  So we undo the previous operations*/
                    if (exactsize)
                        prevp->s.ptr = p;
                    else
                        prevp->s.ptr->s.size += nunits;
                    return NULL;
                }
            }
            blocks[occupied++] = p;
            memInfo.free -= nunits * sizeof(Header);
            memInfo.occupied += nunits * sizeof(Header);
            freep = prevp;
            return (void *) (p + 1);
        }
        if (p == freep)                 // wrapped around free list
            return NULL;
    }
}

void dumbManagerFree(void *ap) {
    Header *freedBlock, *p;
    freedBlock = (Header *) ap - 1;               // Points to block header
    if (!isInBlockArray(freedBlock)) {
        return;
    }
    memInfo.free += freedBlock->s.size * sizeof(Header);
    memInfo.occupied -= freedBlock->s.size * sizeof(Header);
    deleteFromBlockArray(freedBlock);

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

void dumbCreateMemoryManager(void *managedMemory, size_t size) {
    memInfo.totalSize = memInfo.free = size;
    freep = managedMemory;
    freep->s.ptr = managedMemory;
    freep->s.size = size / sizeof(Header);
    blocks = dumbManagerAlloc(sizeof(Header *) * BLOCK);
    blockArraySize = BLOCK;
}

void dumbMemoryInfo(struct memoryInfo *info) {
    info->free = memInfo.free;
    info->totalSize = memInfo.totalSize;
    info->occupied = memInfo.totalSize - memInfo.free;
}

static int growBlockArray() {
    Header **new = dumbManagerAlloc(sizeof(Header **) * (blockArraySize + BLOCK));
    if (new == NULL) {
        return -1;
    }
    memcpy(new, blocks, blockArraySize);
    Header **old = blocks;
    blocks = new;
    blockArraySize += BLOCK;
    dumbManagerFree(old);
    return 0;
}

static int isInBlockArray(Header *block) {
    for (uint64_t i = 0; i < occupied; i++) {
        if (blocks[i] == block)
            return 1;
    }
    return 0;
}

static void deleteFromBlockArray(Header *block) {
    uint64_t i = 0;
    for (; i < occupied; i++) {
        if (blocks[i] == block)
            break;
    }
    for (; i < occupied - 1; i++) {
        blocks[i] = blocks[i + 1];
    }
    occupied--;
}

#endif //BUDDY
