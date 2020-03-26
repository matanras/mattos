#include "vga.h"
#include <stdint.h>
#include <io.h>

#define VGA_BUFFER_ADDR 0xb8000
#define VGA_ROW_SIZE 80 /* size of each row in characters */
#define VGA_ROW_MEM_SIZE 160 /* size of a row in the VGA buffer in bytes */
#define VGA_COLUMN_SIZE 25
#define VGA_PAGE_SIZE 4000

/* screen io ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* indexes pointed by REG_SCREEN_CTRL */
#define CTRL_REG_IDX_CURSOR_POS_HI 0xe
#define CTRL_REG_IDX_CURSOR_POS_LO 0xf

#define MAKE_COLOR_ATTRIBUTE(foreground, background) (((background) << 4) | foreground)

static uint16_t get_cursor_position(void)
{
	uint16_t pos = 0;
	outb(CTRL_REG_IDX_CURSOR_POS_HI, REG_SCREEN_CTRL);
	pos |= ((uint16_t)inb(REG_SCREEN_DATA)) << 8;
	outb(CTRL_REG_IDX_CURSOR_POS_LO, REG_SCREEN_CTRL);
	pos |= inb(REG_SCREEN_DATA);
	
	/* 
	 * VGA hardware reports the cursor position as a number
	 * of characters. We convert it to a character offset into
	 * VGA mem buffer.
	 */
	return pos * 2;
}

static void set_cursor_position(int mem_offset)
{
	int num_of_characters;
	/* convert offset to number of characters */
	num_of_characters = mem_offset / 2;

	outb(CTRL_REG_IDX_CURSOR_POS_HI, REG_SCREEN_CTRL);
	outb((num_of_characters >> 8) & 0xff, REG_SCREEN_DATA);
	outb(CTRL_REG_IDX_CURSOR_POS_LO, REG_SCREEN_CTRL);
	outb(num_of_characters & 0xff, REG_SCREEN_DATA);
}

static inline int screen_coords_to_mem_offset(int column, int row)
{
	return (row * VGA_ROW_SIZE + column) * 2;
}

static inline int get_current_row(int mem_offset)
{
	return mem_offset / (2 * VGA_ROW_SIZE);
}

static int handle_scrolling(int cursor_offset, char attribute)
{
	int cell;
	volatile char *video_mem = (volatile char *)VGA_BUFFER_ADDR;

	if (cursor_offset < VGA_PAGE_SIZE)
		return cursor_offset;

	/*
	 * copy the first vga page starting from the 1st row
	 * a row backwards.
	 */
	memcpy_io(video_mem,
			  video_mem + VGA_ROW_MEM_SIZE,
			  VGA_PAGE_SIZE - VGA_ROW_MEM_SIZE);

	/*last_row = (volatile char *)(VGA_BUFFER_ADDR + VGA_PAGE_SIZE - VGA_ROW_MEM_SIZE);*/
	for (cell = VGA_PAGE_SIZE - VGA_ROW_MEM_SIZE; cell < VGA_PAGE_SIZE; cell += 2) {
		video_mem[cell] = 0;
		video_mem[cell + 1] = attribute;
	}

	/*memset_io(last_row, 0, VGA_ROW_MEM_SIZE);*/
	cursor_offset -= VGA_ROW_MEM_SIZE;
	return cursor_offset;
}

void vga_print_string(const char *str)
{
	vga_print_string_color(str, VGA_COLOR_GRAY, VGA_COLOR_BLACK);
}

void vga_print_string_color(const char *str, enum VGA_COLOR foreground, enum VGA_COLOR background)
{
	char attribute = 0;
	int mem_offset;
	int c;
	volatile char *video_mem = (volatile char *)VGA_BUFFER_ADDR;

	attribute = MAKE_COLOR_ATTRIBUTE(foreground, background);

	mem_offset = get_cursor_position();
	c = *str;

	while (c != '\0') {
		
		if (c != '\n') {
			video_mem[mem_offset++] = c;
			video_mem[mem_offset++] = attribute;
		}
		else {
			mem_offset += VGA_ROW_MEM_SIZE - (mem_offset % VGA_ROW_MEM_SIZE);
		}

		mem_offset = handle_scrolling(mem_offset, attribute);
		c = *++str;
	}

	set_cursor_position(mem_offset);
}

void vga_print_char_at(char c, int column, int row,
				   enum VGA_COLOR foreground, enum VGA_COLOR background)
{
	char attribute = 0;
	int mem_offset;
	volatile char *video_mem = (volatile char *)VGA_BUFFER_ADDR;

	attribute = (background << 4);
	attribute |= foreground;

	if (column >= 0 && row >= 0)
		mem_offset = screen_coords_to_mem_offset(column, row);
	else
		mem_offset = get_cursor_position();

	if (c == '\n') {
		/* we set the offset to the end of the row. */
		mem_offset = screen_coords_to_mem_offset(VGA_ROW_SIZE - 1, mem_offset / (2 * VGA_ROW_SIZE));
	}
	else {
		video_mem[mem_offset] = c;
		video_mem[mem_offset + 1] = attribute;
	}

	mem_offset += 2;
	set_cursor_position(mem_offset);
}

void vga_clear_screen(void)
{
	int row;
	int col;
	int mem_offset;
	volatile char *video_mem = (volatile char *)VGA_BUFFER_ADDR;

	for (row = 0; row < VGA_COLUMN_SIZE; ++row) {
		for (col = 0; col < VGA_ROW_SIZE; ++col) {
			mem_offset = screen_coords_to_mem_offset(col, row);
			video_mem[mem_offset] = ' ';
			video_mem[mem_offset + 1] = MAKE_COLOR_ATTRIBUTE(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
		}
	}

	set_cursor_position(0);
}
