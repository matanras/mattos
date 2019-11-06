#ifndef _DRIVERS_SCREEN_H
#define _DRIVERS_SCREEN_H

enum VGA_COLOR {
	VGA_COLOR_BLACK,
	VGA_COLOR_BLUE,
	VGA_COLOR_GREEN,
	VGA_COLOR_CYAN,
	VGA_COLOR_RED,
	VGA_COLOR_PURPLE,
	VGA_COLOR_BROWN,
	VGA_COLOR_GRAY,
	VGA_COLOR_DARK_GRAY,
	VGA_COLOR_LIGHT_BLUE,
	VGA_COLOR_LIGHT_GREEN,
	VGA_COLOR_LIGHT_CYAN,
	VGA_COLOR_LIGHT_RED,
	VGA_COLOR_LIGHT_PURPLE,
	VGA_COLOR_YELLOW,
	VGA_COLOR_WHITE
};

void vga_print_string(const char *str);

void vga_print_string_color(const char *str, enum VGA_COLOR foreground, enum VGA_COLOR background);

void vga_print_char_at(char c, int column, int row,
				   enum VGA_COLOR foreground, enum VGA_COLOR background);

void vga_clear_screen(void);

#endif /* _DRIVERS_SCREEN_H */