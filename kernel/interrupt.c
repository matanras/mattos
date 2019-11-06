#include <interrupt.h>
#include <idt.h>
#include <i8259a.h>
#include <vga.h>

void init_interrupts(void)
{
    if (!probe_8259A())
        return;

    native_disable_interrupts();
    init_8259A(ISA_INTERUPTS_VECTOR, ISA_INTERUPTS_VECTOR + 8);
    idt_load();
    native_enable_interrupts();
}