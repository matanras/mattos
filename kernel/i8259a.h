/*
 * This header includes definitions for controlling the 8259A PIC.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

enum i8259A_type {
    i8259A_MASTER,
    i8259_SLAVE
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

int i8259A_get_mask(enum i8259A_type type);

bool i8259A_probe(void);

void i8259A_ack_irq(enum i8259A_type type);