#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

void idt_add_interrupt_entry(int entry, void *isr);

void idt_load(void);

#endif /* _IDT_H */