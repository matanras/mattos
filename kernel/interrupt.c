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

void interrupt_add(int vector, interrupt_handler_t handler)
{
    idt_add_interrupt_entry(vector, handler);
    enum i8259A_type master_or_slave;
    
    if (vector > ISA_INTERUPTS_VECTOR && vector < ISA_INTERUPTS_VECTOR + 8) {
        master_or_slave = i8259A_MASTER;
    }
    else {
        master_or_slave = i8259_SLAVE;
    }

    int current_mask = i8259A_get_mask(master_or_slave);
    // TODO: add generic calculation of mask.
    i8259A_set_mask(master_or_slave, current_mask & ~(1 << (vector - ISA_INTERUPTS_VECTOR)));
}