#define VGA_BUFFER_ADDR 0xb8000
#define VGA_ATTRIBUTE_WHITE_ON_BLACK 0x0f

void main(void)
{
	char *vga_buffer = (char *) VGA_BUFFER_ADDR;
	vga_buffer[0] = 'X';
	vga_buffer[1] = VGA_ATTRIBUTE_WHITE_ON_BLACK;
	vga_buffer[2] = 'A';
	vga_buffer[3] = VGA_ATTRIBUTE_WHITE_ON_BLACK;
	vga_buffer[4] = 'B';
	vga_buffer[5] = VGA_ATTRIBUTE_WHITE_ON_BLACK;
	vga_buffer[5] = VGA_ATTRIBUTE_WHITE_ON_BLACK;
	
}