#include <exceptions.h>
#include <vga.h>

__interrupt void divide_by_zero_fault(struct interrupt_frame *ir_frame)
{
    for (int i = 0; i < 100000; ++i)
        vga_print_string("HELLO\n");
}

__interrupt void double_fault(struct interrupt_frame *ir_frame)
{
    vga_print_string("DOUBLE FAULT\n");
}