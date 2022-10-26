include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
MODULE-ELF=0000-sampleCodeModule.elf
SOURCES=$(wildcard *.c)
SOUCES_SHELL=$(wildcard Shell/*.c)
SOURCES_LIBS=$(wildcard Libs/*.c)
SOURCES_TESTS=$(wildcard Tests/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHELL=$(SOUCES_SHELL:.c=.o)
OBJECTS_LIBS=$(SOURCES_LIBS:.c=.o)
OBJECTS_TESTS=$(SOURCES_TESTS:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

STATICLIBS=

all: $(MODULE)

# Apparently, the order of the objects is important for the linkeditor :(
gdb: GCCFLAGS := -g $(GCCFLAGS)
gdb: $(MODULE)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 -o ../$(MODULE-ELF) $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_LIBS) $(OBJECTS_ASM) $(OBJECTS_TESTS) $(STATICLIBS)

$(MODULE): $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_LIBS) $(OBJECTS_ASM) $(OBJECTS_TESTS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_LIBS) $(OBJECTS_ASM) $(OBJECTS_TESTS) $(STATICLIBS)

clean:
	rm -rf *.o ./asm/*.o 

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

.PHONY: all clean print
