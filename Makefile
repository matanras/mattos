CC = gcc
C_SOURCES = $(wildcard  kernel/*.c drivers/*.c)
HEADERS = $(wildcard  kernel/*.h drivers/*.h)
OBJ = $(C_SOURCES:.c=.o)

all: os-image

os-image: bootsector.bin kernel.bin
	cat $^ > $@

# Build  the  kernel binary
kernel.bin: kernel/kernel_entry.o $(OBJ)
	ld -e main -Ttext 0x1000 $^ -o $@ --oformat binary -m elf_i386

%.o: %.c $(HEADERS)
	$(CC) -fno-pie -ffreestanding -m32 -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

bootsector.bin: boot/bootsector.asm boot/string.asm
	nasm -f bin -I boot/ $< -o $@

clean:
	rm -f os-image
	rm -f *.bin *.o
	
run: all
	qemu-system-x86_64 os-image