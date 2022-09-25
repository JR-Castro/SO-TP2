include Makefile.inc

SAMPLE_DATA=0001-sampleDataModule.bin

all: sampleCodeModule sampleDataModule

gdb: GCCFLAGS := -g $(GCCFLAGS)
gdb: sampleCodeModulegdb sampleDataModule

sampleCodeModule:
	cd SampleCodeModule; make

sampleCodeModulegdb:
	cd SampleCodeModule; make gdb

sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd SampleCodeModule; make clean
	rm -rf *.bin *.elf


.PHONY: sampleCodeModule sampleCodeModulegdb all clean
