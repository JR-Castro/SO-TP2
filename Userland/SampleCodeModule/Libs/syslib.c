// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/syslib.h"

void getTime(){
    kernelTime_t time;
    char buffer[64];
    sys_time(&time);

    putchar('\n');
    uintToBase(time.hours, buffer, 10);
    fputs(buffer, STDOUT);
    putchar(':');
    uintToBase(time.minutes, buffer, 10);
    fputs(buffer, STDOUT);
    putchar(':');
    uintToBase(time.seconds, buffer, 10);
    puts(buffer);

    uintToBase(time.day, buffer, 10);
    fputs(buffer, STDOUT);
    putchar('/');
    uintToBase(time.month, buffer, 10);
    fputs(buffer, STDOUT);
    putchar('/');
    uintToBase(time.year+2000, buffer, 10);
    puts(buffer);

}

//Retorna 1 si lo encuentra, 0 sino
unsigned int charBelongs(char *s,char c){
    while(*s != '\0'){
        if(*s == c){
            return 1;
        }
        s++;
    }
    return 0;
}

void excepDivZero(){
    div_zero();
}

void excepInvalidOpcode(){
    inv_opcode();
}

static void reverse(char s[]);

//Turn an integer number to a char array
void itoa(int n, char s[]){
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}

static void reverse(char s[]){
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}

//Tomado de x86-Barebones
uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void *lib_memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    while (n--)
        *p++ = (unsigned char)c;
    return s;
}

void acquire(int *lock) {
    while (_xchg(lock, 1) != 0);
}

void release(int *lock) {
    _xchg(lock,0);
}
