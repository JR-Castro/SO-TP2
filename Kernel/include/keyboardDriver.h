#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include "idtLoader.h"
#include "inforeg.h"
#include "lib.h"
#include "naiveConsole.h"
#include "scheduler.h"
#include "semaphores.h"

void keyboard_handler(uint64_t *registers);

uint64_t readBuffer(char *output, uint64_t count);

void initKeyboard(); 

#endif
