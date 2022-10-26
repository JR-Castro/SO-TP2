// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef UINT64LISTADT_H
#define UINT64LISTADT_H

#include <stdint.h>
#include "defs.h"
#include "memManager.h"

typedef struct uint64Node {
    uint64_t val;
    struct uint64Node *next;
} uint64Node_t;

typedef struct uint64List {
    uint64Node_t *first, *last;
} uint64List_t;

/* Adds a node with the specified pid at the end of the list
 * Returns: 0 if successful
 *          -1 if memory allocation failed
 */
int uint64ListAddNode(uint64List_t *list, uint64_t pid);

/* Searches specified uint64List for node with matching pid
 * Returns: Pointer to node if found
 *          NULL otherwise
 */
uint64Node_t *uint64ListSearch(uint64List_t *list, uint64_t pid);

/* Deletes pid from list of PIDs
 * Returns: 0 if sucessfully deleted
 *          1 if PID wasn't found
 */
int uint64ListRemoveNode(uint64List_t *list, uint64_t pid);

/* Returns: pid of first node if list is not empty
 *          0 if list is empty
 */
uint64_t uint64ListGetFirst(uint64List_t *list);

/* Frees all allocated memory in a uint64List */
void uint64ListFree(uint64List_t *list);



#endif //UINT64LISTADT_H
