#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <stdint.h>
#include <kernel.h>

__packed struct interrupt_frame {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

typedef void(*interrupt_handler_t)(struct interrupt_frame *frame);

#define __interrupt __attribute__((interrupt))

#define CPU_INTERRUPTS_VECTOR		0x00 /* irqs 0x00-0x1f */

#define EXTERNAL_INTERRUPTS_VECTOR 	0x20 /* external devices irqs */

#define ISA_INTERUPTS_VECTOR		0x20 /* 0x20 - 0x30 */

static inline void native_disable_interrupts(void)
{
    asm volatile("cli");
}

static inline void native_enable_interrupts(void)
{
    asm volatile("sti");
}

void init_interrupts(void);

void interrupt_add(int vector, interrupt_handler_t handler);

#endif /* _INTERRUPTS_H */