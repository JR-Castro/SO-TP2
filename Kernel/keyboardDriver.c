// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/keyboardDriver.h"
#include "include/semaphores.h"
#define KEYBOARD_SEM "KEYBOARD_SEM"

#define BUFFER_LENGTH 100

static char keys[] = {
    0,  // Error
    27, // Escape
    '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    8,    // Backspace
    '\t', // Tab
    'q',  'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']',
    '\n', // Enter
    0,    // LControl
    'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, // LShift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, // RShift
    '*',
    0, // LAlt
    ' ',
    0,                                               // CapsLock
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0, // F1-F10
    0,                                               // NumLock
    0,                                               // ScrollLock
    '7',  '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',  '0', '.'};

static char capKeys[] = {
    0,  // Error
    27, // Escape
    '1',  '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    8,    // Backspace
    '\t', // Tab
    'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[',  ']',
    '\n', // Enter
    0,    // LControl
    'A',  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
    0, // LShift
    '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/',
    0, // RShift
    '*',
    0, // LAlt
    ' ',
    0,                                               // CapsLock
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0, // F1-F10
    0,                                               // NumLock
    0,                                               // ScrollLock
    '7',  '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',  '0', '.'};

static char shiftCapKeys[] = {
    0,  // Error
    27, // Escape
    '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',  '+',
    8, // Backspace
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '{', '}',
    '\n', // Enter
    0,    // LControl
    'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~',
    0, // LShift
    '|',  'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?',
    0, // RShift
    '*',
    0, // LAlt
    ' ',
    0,                                               // CapsLock
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0, // F1-F10
    0,                                               // NumLock
    0,                                               // ScrollLock
    '7',  '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',  '0', '.'};

static char shiftedKeys[] = {
    0,  // Error
    27, // Escape
    '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',  '+',
    8, // Backspace
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',  '{', '}',
    '\n', // Enter
    0,    // LControl
    'A',  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    0, // LShift
    '|',  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0, // RShift
    '*',
    0, // LAlt
    ' ',
    0,                                               // CapsLock
    0,    0,   0,   0,   0,   0,   0,   0,   0,   0, // F1-F10
    0,                                               // NumLock
    0,                                               // ScrollLock
    '7',  '8', '9', '-', '4', '5', '6', '+', '1', '2', '3',  '0', '.'};

static sem_t *keySem = NULL;
char buffer[BUFFER_LENGTH];
uint64_t read = 0;
uint64_t writen = 0;
static int shift = 0, capsLock = 0, control = 0, alt = 0;

static void add(char key);
static char translate(uint16_t key);
static uint8_t pressed(uint16_t scancode, uint16_t key);

void initKeyboard() {
  
  while ((keySem = sem_open(KEYBOARD_SEM, 0, 0)) == NULL);

}

void keyboard_handler(uint64_t *registers) {
  if (!(read_port(0x64) & 0x01))
    return;
  uint16_t scancode = read_port(0x60);
  uint16_t key = scancode & 0x7F;
  if (pressed(scancode, key)) {
    if (control) {
      if (key == 0x2E) { // Ctrl+C = copy registers
        setRegisters(registers);
      }
    } else {
      add(translate(key));
    }
  }
}

static uint8_t pressed(uint16_t scancode, uint16_t key) {
  if ((scancode & 0x80) == 0x80) {
    switch (key) {
    case 0x1d:
      control = 0;
      break;
    case 0x2a:
    case 0x36:
      shift = 0;
      break;
    case 0x38:
      alt = 0;
      break;
    default:
      break;
    }
  } else {
    switch (key) {
    case 0x1d:
      control = 1;
      break;
    case 0x3a:
      capsLock = !capsLock;
      break;
    case 0x2a:
    case 0x36:
      shift = 1;
      break;
    case 0x38:
      alt = 1;
      break;
    default:
      return 1;
      break;
    }
  }
  return 0;
}

static char translate(uint16_t key) {
  if (capsLock && !shift)
    return capKeys[key];
  else if (capsLock && shift)
    return shiftCapKeys[key];
  else if (!capsLock && !shift)
    return keys[key];
  else
    return shiftedKeys[key];
}

static void add(char key) {
  buffer[writen % BUFFER_LENGTH] = key;
  writen++;
  sem_post(KEYBOARD_SEM);
}

uint64_t readBuffer(char *output, uint64_t count) {
  uint64_t i = 0;
  
  for (; i < count; i++) {
    sem_wait(KEYBOARD_SEM);
    output[i] = buffer[read % BUFFER_LENGTH];
    read++;
  }

  return i;
}
