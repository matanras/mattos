all: os-image

os-image: bootsector.bin kernel.bin
	cat $^ > $@

# Build  the  kernel binary
kernel.bin: kernel_entry.o kernel.o
	ld -e main -Ttext 0x1000 $^ -o $@ --oformat binary -m elf_i386

# Build  the  kernel  object  file
kernel.o: kernel_main.c
	gcc -fno-pie -ffreestanding -m32 -c $< -o $@

# Build  the  kernel  entry  object  file.
kernel_entry.o: kernel_entry.asm
	nasm $< -f elf -o $@

bootsector.bin: bootsector.asm
	nasm -f bin $< -o $@

clean:
	rm *.bin *.o
	
run: all
	qemu-system-x86_64 os-image