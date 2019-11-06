CC = gcc
LD = ld
CFLAGS = -I"drivers/"			\
		 -I"kernel/"			\
		 -mgeneral-regs-only	\
		 -mno-red-zone			\

C_SOURCES = $(wildcard  kernel/*.c drivers/*.c)
HEADERS = $(wildcard  kernel/*.h drivers/*.h)
OBJ = $(C_SOURCES:.c=.o)

.PHONY: all
all: os-image

os-image: bootsector.bin kernel.bin
	@cat $^ > $@

# Build  the  kernel binary
kernel.bin: kernel/kernel_entry.o $(OBJ)
	@$(LD) -e main -Ttext 0x1000 $^ -o $@ --oformat binary -m elf_i386

%.o: %.c $(HEADERS)
	@$(CC) -fno-pie -ffreestanding -m32 -c $< -o $@ $(CFLAGS)

%.o: %.asm
	@nasm $< -f elf -o $@

bootsector.bin: boot/bootsector.asm boot/string.asm
	@nasm -f bin -I boot/ $< -o $@

.PHONY: clean
clean:
	@find ./ -type f \( -iname \*.o -o -iname \*.bin -o -iname os-image \) -delete

.PHONY: run
run: all
	@qemu-system-x86_64 -d int os-image