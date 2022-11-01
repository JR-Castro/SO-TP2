// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* sampleCodeModule.c */

#include "include/bash.h"
#include "include/shell.h"

char * v = (char*)0xB8000 + 79 * 2;

int main() {
    char * argv[] = {"Shell"};
    sys_createProcess((void (*)(int, char **))shell , 1, argv);
	return 0x600DB7E;
}