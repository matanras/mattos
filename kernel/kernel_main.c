#include <vga.h>
#include <interrupt.h>

void main(void)
{
	vga_print_string("Kernel init started.\n");
	init_interrupts();



	while (1);

	/* while (1); */
} 