CC = gcc
LD = ld
INCLUDES = 	-I"drivers/"	\
	 		-I"kernel/"
	
	
CFLAGS = -mgeneral-regs-only	\
	 	 -mno-red-zone			\
	 	 -ffreestanding			\
	 	 -m32					\
	 	 -fno-pie				\
		 -Wall					\
		 -Werror				\
		 $(INCLUDES)

C_SOURCES = $(wildcard  kernel/*.c drivers/*.c)
ASM_SOURCES = $(wildcard kernel/*.asm drivers/*.asm)
HEADERS = $(wildcard  kernel/*.h drivers/*.h)
OBJ = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

.PHONY: all
all: os-image

os-image: bootsector.o $(OBJ)
	@$(LD) -T linker.ld -o $@ -m elf_i386 $^

%.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.asm
	@nasm -f elf -o $@ $<

bootsector.o: boot/bootsector.asm boot/string.asm
	@nasm -f elf -I boot/ $< -o $@

.PHONY: clean
clean:
	@find ./ -type f \( -iname \*.o -o -iname \*.bin -o -iname os-image \) -delete

.PHONY: run
run: all
	@qemu-system-x86_64 os-image
