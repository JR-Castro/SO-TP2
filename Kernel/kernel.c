// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "include/lib.h"
#include "include/moduleLoader.h"
#include "include/naiveConsole.h"
#include "include/idtLoader.h"
#include "include/memManagerADT.h"
#include "include/scheduler.h"
#include "include/keyboardDriver.h"

#define MEMORY_START ((void*)0x600000)
#define MEMORY_SIZE 134217728   //128MB

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

// PVS note about constant to pointer
static void *const sampleCodeModuleAddress = (void *) 0x400000;
static void *const sampleDataModuleAddress = (void *) 0x500000;

typedef int (*EntryPoint)();


void clearBSS(void *bssAddress, uint64_t bssSize) {
    memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
    return (void *) (
            (uint64_t) & endOfKernel
                         + PageSize * 8                //The size of the stack itself, 32KiB
                         - sizeof(uint64_t)            //Begin at the top of the stack
    );
}

void *initializeKernelBinary() {
    void *moduleAddresses[] = {
            sampleCodeModuleAddress,
            sampleDataModuleAddress
    };

    loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);

    return getStackBase();
}

int main() {
    ncClear();
    initializeMemoryManager(MEMORY_START, MEMORY_SIZE);
    initializeScheduler();
    initKeyboard();
    load_idt();
    ((EntryPoint) sampleCodeModuleAddress)();
    _sti();
    while (1)
        _hlt();
//	loadUserland(sampleCodeModuleAddress, (uint64_t*) 0x900000);
    ncPrint("[Finished]");
    return 0;
}
