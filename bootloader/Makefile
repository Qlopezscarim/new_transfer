all: x86-mini.bin arm-mini.bin

x86-mini.bin: x86-mini.elf
	objcopy -O binary -j .text x86-mini.elf x86-mini.bin
	
arm-mini.bin: arm-mini.elf
	objcopy -O binary -j .text -I elf32-little arm-mini.elf arm-mini.bin

x86-mini.elf: x86-mini.o
	ld -m elf_i386 -o x86-mini.elf -g --section-start=.text=0x7c00 --section-start=.data=0x0500 x86-mini.o -e _start 

x86-mini.o: x86-mini.s 
	as -o x86-mini.o --march=i386 --32 x86-mini.s

arm-mini.elf: arm-mini.o
	arm-none-eabi-ld -o arm-mini.elf -g arm-mini.o -e _start 
	
arm-mini.o: arm-mini.s 
	arm-none-eabi-as -march=armv3 -o arm-mini.o arm-mini.s
	
clean: arm-mini.elf arm-mini.o x86-mini.elf x86-mini.o
	rm arm-mini.elf arm-mini.o x86-mini.elf x86-mini.o
