// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/lib.h"

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void *memmove(void *str1, const void *str2, size_t n) {
    char *dest = (char *) str1;
    const char *src = (const char *) str2;
    if (dest < src) {
        for (size_t i = 0; i < n; i++) {
            dest[i] = src[i];
        }
    } else {
        for (size_t i = n; i != 0; i--) {
            dest[i - 1] = src[i - 1];
        }
    }
    return str1;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int strlen(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return ret;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (*src && n) {
        *dest = *src;
        dest++;
        src++;
        n--;
    }
    *dest = '\0';
    return ret;
}

char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest) dest++;
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return ret;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (*dest) dest++;
    while (*src && n) {
        *dest = *src;
        dest++;
        src++;
        n--;
    }
    *dest = '\0';
    return ret;
}

#define STRINGBLOCK 100

/*  Grows a resizeable string   */
static int growString(char **str, int *size) {
    char *new = memAlloc(*size + STRINGBLOCK);
    if (!new) {
        return -1;
    }
    memcpy(new, *str, *size);
    memFree(*str);
    *str = new;
    return 0;
}

/*  Appends a string to a resizeable string, returns index of end of string   */
int copyResizeableString(char **s, const char* src, int *size, int start) {
    if (s == NULL)
        return -1;
    if (*s == NULL) {
        *s = memAlloc(STRINGBLOCK);
        if (!*s)
            return -1;
        *size = STRINGBLOCK;
    }
    int i = start;
    for (int j = 0; src[j]; ++i, ++j) {
        if (i >= *size - 1) {
            if (growString(s, size))
                return i;
        }
        (*s)[i] = src[j];
    }
    (*s)[i] = '\0';
    return i;
}

int finishResizeableString(char **s, int finish) {
    char *new = memAlloc(finish);
    if (!new) {
        return -1;
    }
    memcpy(new, *s, finish + 1);
    memFree(*s);
    *s = new;
    return 0;
}

#undef STRINGBLOCK
