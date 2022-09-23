// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* sampleCodeModule.c */

#include <bash.h>

char * v = (char*)0xB8000 + 79 * 2;

int main() {
	bash();
	return 0x600DB7E;
}