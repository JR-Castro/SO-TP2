include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
#SOURCES=$(wildcard [^_]*.c)
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

STATICLIBS=

all: $(MODULE)

# Apparently, the order of the objects is important for the linkeditor :(
gdb: GCCFLAGS := -g $(GCCFLAGS)
gdb: $(MODULE)
#	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 -o ../0000-sampleCodeModule.elf $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 -o ../0000-sampleCodeModule.elf  _loader.o test_util.o syslib.o bash.o sampleCodeModule.o test_mm.o asm/syslib.o

$(MODULE): $(OBJECTS) $(OBJECTS_ASM)
#	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld -o ../$(MODULE)	_loader.o test_util.o syslib.o bash.o sampleCodeModule.o test_mm.o asm/syslib.o
#	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) -o ../$(MODULE)
#	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(SOURCES) -o ../0000-sampleCodeModule.elf

clean:
	rm -rf *.o ./asm/*.o 

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

.PHONY: all clean print
