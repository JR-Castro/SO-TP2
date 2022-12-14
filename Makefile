
all:  bootloader kernel userland image

gdb: bootloader
	cd Kernel; make MM=$(MM) gdb
	cd Userland; make gdb
	cd Image; make all

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

pvs: clean
	./pvs.sh $(MM)

.PHONY: bootloader image collections kernel gdb userland all clean
