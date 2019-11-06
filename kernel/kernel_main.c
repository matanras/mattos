#include <vga.h>
#include <interrupt.h>

void char_to_hex(char c, char hex_str[3])
{
	char high_nibble, low_nibble;
	high_nibble = (c & 0xf0) >> 4;
	low_nibble = c & 0xf;

	if (high_nibble < 9)
		hex_str[0] = (char)(0x30 + high_nibble);
	else
		hex_str[0] = (char)(0x57 + high_nibble);

	if (low_nibble < 9)
		hex_str[1] = (char)(0x30 + low_nibble);
	else
		hex_str[1] = (char)(0x57 + low_nibble);

	hex_str = '\0';
}

char arr[8];

#include <kernel.h>

__attribute__((interrupt())) void handle_keyboard(struct interrupt_frame *frame)
{
	vga_print_string("key.");
}
#include <i8259a.h>
void main(void)
{
	init_interrupts();
	shutdown_8259A();
	/* while (1); */
	vga_print_string("test");
	asm volatile("int $0");
} 