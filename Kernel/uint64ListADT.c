// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/uint64ListADT.h"

int uint64ListAddNode(uint64List_t *list, uint64_t pid) {
    uint64Node_t *newNode = (uint64Node_t *) memAlloc(sizeof(uint64Node_t));
    if (newNode == NULL)
        return -1;

    newNode->val = pid;

    if (list->first == NULL)
        list->first = newNode;
    else
        list->last->next = newNode;
    list->last = newNode;
    newNode->next = NULL;

    return 0;
}

uint64Node_t *uint64ListSearch(uint64List_t *list, uint64_t pid) {
    uint64Node_t *current = list->first;
    while (current != NULL) {
        if (current->val == pid)
            break;
        current = current->next;
    }
    return current;
}

int uint64ListRemoveNode(uint64List_t *list, uint64_t pid) {
    uint64Node_t *previous = NULL, *current = list->first;
    while (current != NULL) {
        if (current->val == pid)
            break;
        previous = current;
        current = current->next;
    }
    if (current == NULL)
        return 1;   // Nothing to delete

    if (previous == NULL)
        list->first = current->next;
    else
        previous->next = current->next;
    if (current == list->last)
        list->last = previous;
    memFree(current);
    return 0;
}

uint64_t uint64ListGetFirst(uint64List_t *list) {
    if (list->first == NULL)
        return 0;
    uint64_t pid = list->first->val;
    uint64Node_t *first = list->first;
    list->first = list->first->next;
    memFree(first);
    return pid;
}

void uint64ListFree(uint64List_t *list) {
    uint64Node_t *current = list->first, *next;
    while (current != NULL) {
        next = current->next;
        memFree(current);
        current = next;
    }
}
