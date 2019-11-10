#include <interrupt.h>
#include <idt.h>
#include <i8259a.h>
#include <vga.h>

void init_interrupts(void)
{
    if (!i8259A_probe())
        return;

    native_disable_interrupts();
    i8259A_init(ISA_INTERUPTS_VECTOR, ISA_INTERUPTS_VECTOR + 8);
    idt_load();
    native_enable_interrupts();
}