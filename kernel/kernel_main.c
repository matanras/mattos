#include <vga.h>
#include <keyboard.h>
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
// #include <io.h>
// #include <i8259a.h>
// __interrupt void handler(struct interrupt_frame *frame)
// {
// 	(void)inb(0x60);
// 	vga_print_string("helo\n");
// 	i8259A_ack_irq(i8259A_MASTER);
// }

void main(void)
{
	vga_print_string("Kernel init started.\n");
	init_interrupts();

	if (!keyboard_init()) {
		vga_print_string("Failed to initialize keyboard\n");
	}
	// interrupt_add(ISA_INTERUPTS_VECTOR + 1, handler);
	// i8259A_set_mask(i8259A_MASTER, ~2);
	while (1);

	/* while (1); */
} 