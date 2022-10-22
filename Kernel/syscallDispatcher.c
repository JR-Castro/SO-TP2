// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscallDispatcher.h>
#include <scheduler.h>

static uint64_t sys_read(unsigned int fd,char* output, uint64_t count);
static void sys_write(unsigned fd,const char* buffer, uint64_t count);
static void sys_time(time_t * s);
static void sys_copymem(uint64_t address, uint8_t * buffer, uint64_t length);

uint64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rax, uint64_t * registers){
    switch(rax){
        case 0:
            return sys_read((unsigned int)rdi, (char*)rsi,rdx);
            break;
        case 1:
            sys_write((unsigned int)rdi, (char*)rsi,rdx);
            break;
        case 2:
            return (uint64_t) getRegisters((uint64_t *)rdi);
        case 3:
            return createProcess((void (*)(int, char**))rdi, (int)rsi, (char**)rdx);
        case 5:
            sys_time((time_t*)rdi);
            break;
        case 6:
            sys_copymem(rdi,(uint8_t *) rsi, rdx);
            break;
        default:
            break;
    }
    return 0;
}

static uint64_t sys_read(unsigned int fd,char* output, uint64_t count){
    switch (fd)
    {
    case STDIN:
        return readBuffer(output, count);
        break;
    
    default:
        return 0;
    }
}

static void sys_write(unsigned fd,const char* buffer, uint64_t count){
    uint64_t i = 0;
    while (i < count)
    {
        switch(fd){
            case STDOUT:
                ncPrintChar(buffer[i]);
                break;
            case STDERR:
                ncPrintCharFormat(buffer[i], ERROR_FORMAT);
                break;
            default:
                return;
        }
        i++;
    }
}

static void sys_time(time_t * s){
    s->day = localDay();
    s->month = localMonth();
    s->year = localYear();
    s->hours = localHours();
    s->minutes = getMinutes();
    s->seconds = getSeconds();
}

static void sys_copymem(uint64_t address, uint8_t * buffer, uint64_t length){
    memcpy((void*)buffer, (void*)address, length);
}
