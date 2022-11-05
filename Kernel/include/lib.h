#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stddef.h>

#include "memManagerADT.h"

void *memset(void *destination, int32_t character, uint64_t length);

void *memcpy(void *destination, const void *source, uint64_t length);

int strlen(const char *s);

int strcmp(const char *s1, const char *s2);

int strlen(const char *s);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *src, size_t n);

char *strcat(char *dest, const char *src);

char *strncat(char *dest, const char *src, size_t n);

void *memmove(void *str1, const void *str2, size_t n);

/*  Appends a string to a resizeable string, returns index of end of string   */
int copyResizeableString(char **s, const char* src, int *size, int start);

int finishResizeableString(char **s, int finish);

uint8_t getSeconds();

uint8_t getMinutes();

uint8_t getHours();

uint8_t getDay();

uint8_t getMonth();

uint8_t getYear();

uint8_t read_port(uint8_t key);

char *cpuVendor(char *result);

void tick();

void loadUserland(uint64_t *address, uint64_t *rsp);

#endif