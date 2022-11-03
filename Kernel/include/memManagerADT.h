// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SO_TP2_MEMMANAGERADT_H
#define SO_TP2_MEMMANAGERADT_H

#include "lib.h"

void initializeMemoryManager(void *start, uint64_t size);

void *memAlloc(size_t nbytes);

void memFree(void *ap);

#endif //SO_TP2_MEMMANAGERADT_H
