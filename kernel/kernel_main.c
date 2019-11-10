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
#include <idt.h>
#include <interrupt.h>
#include <i8259a.h>
#include <io.h>

__attribute__((interrupt())) void handle_keyboard(struct interrupt_frame *frame)
{
	uint8_t key = inb(0x60);
	vga_print_string("key.\n");
	i8259A_ack_irq(i8925A_MASTER);
}

void main(void)
{
	init_interrupts();
	vga_print_string("registered keyboard irq handler.\n");
	idt_add_interrupt_entry(ISA_INTERUPTS_VECTOR + 1, handle_keyboard);
	i8259A_set_mask(i8925A_MASTER, 0xfd);
	/* while (1); */
} 