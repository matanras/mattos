/*
 * This header includes definitions for controlling the 8259A PIC.
 */

#ifndef _8259A_H
#define _8259A_H

#include <io.h>
#include <stdbool.h>

#define PIC_MASTER_A0_0		0x20 /* master pic address with A0=0. */
#define PIC_MASTER_A0_1		0x21 /* master pic address with A0=1. */
#define PIC_MASTER_IMR      PIC_MASTER_A0_1
#define PIC_SLAVE_A0_0	 	0xa0 /* slave pic address with A0=0. */
#define PIC_SLAVE_A0_1		0xa1 /* slave pic address with A0=0. */
#define PIC_SLAVE_IMR		PIC_SLAVE_A0_1

#define PIC_CASCADE_IR   	2

/*
 * 8259A initialization sequence consists of a set of Initialization Command Words (ICWs).
 * The first ICW (ICW1) is sent with A0=0 (first bit of address line is 0), so we use PIC_MASTER_A0_0.
 * In ICW2 we set the offset of the interrupt vector by sending the offset with A0=1, so we use PIC_MASTER_A0_1.
 * For a master 8259A, ICW3 specifies which interrupt line is connected to the slave PIC (8259A-2).
 * For 8259A-2, ICW3 specifies the slave ID.
 * Notice that all of this is only true in cascade mode (master/slave), which is always the case
 * in an IBM PC compatible system.
 * ICW4, if needed (specified by a bit in ICW1), controls the Automatic End of Interrupt (AEOI) mechanism,
 * and also tells the specified PIC if it's a master or a slave.
 * ICW3 and ICW4 are also sent with A1=1.
 */

#define PIC_ICW1_DEFAULT 	  				16 /* cascade mode, edge triggered */
#define PIC_ICW1_ICW4_NEEDED    			 1 /* specifies ICW4 is needed */
#define PIC_ICW3_SLAVE_AT_IRQ2 				(1 << PIC_CASCADE_IR)
#define PIC_ICW3_SLAVE_ID	   				 2 /* specified the slave ID given to 8259A-2 */
#define PIC_ICW4_8086_MODE	   				 1
#define PIC_ICW4_AEOI_ENABLED  				 2
#define PIC_ICW4_SLAVE						 8 /* tells the PIC it's the slave */
#define PIC_ICW4_MASTER						 9 /* tells the PIC it's the master */
#define PIC_ICW4_SPECIAL_FULLY_NESTED_MODE  16

#define PIC_OCW2_EOI                        32 /* non-specific EOI */

enum i8259A_type {
    i8925A_MASTER,
    i8925A_SLAVE
};

/**
 * init_8259A - initialize the 8259A PIC and map the IRQ lines of the master
 * and the slave.
 * @master_offset: IDT offset to which the master's IRQs will be mapped.
 * @slave_offset: IDT offset to which the slave's IRQs will be mapped.
 */
void i8259A_init(int master_offset, int slave_offset);

void i8259A_shutdown(void);

void i8259A_set_mask(enum i8259A_type type, uint8_t mask);

bool i8259A_probe(void);

void i8259A_ack_irq(enum i8259A_type type);

#endif /* _8259A_H */