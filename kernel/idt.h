#pragma once

#include <stdint.h>

void idt_add_interrupt_entry(int entry, void *isr);

void idt_load(void);
