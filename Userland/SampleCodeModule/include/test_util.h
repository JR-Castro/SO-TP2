// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef SO_TP2_TEST_UTIL_H
#define SO_TP2_TEST_UTIL_H

#include "syslib.h"
#include <stdint.h>

uint32_t GetUint();

uint32_t GetUniform(uint32_t max);

uint8_t memcheck(void *start, uint8_t value, uint32_t size);

int64_t satoi(char *str);

void bussy_wait(uint64_t n);

void endless_loop();

void endless_loop_print(uint64_t wait);

#endif //SO_TP2_TEST_UTIL_H
