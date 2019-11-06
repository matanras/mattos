#include "i8259a.h"

void init_8259A(int master_offset, int slave_offset)
{
	uint8_t prev_master_mask;
	uint8_t prev_slave_mask;

	/*prev_master_mask = get_8259A_imr(true);
	prev_slave_mask = get_8259A_imr(false);*/

	outb_p(PIC_ICW1_DEFAULT | PIC_ICW1_ICW4_NEEDED, PIC_MASTER_A0_0);
	outb_p(master_offset, PIC_MASTER_A0_1);
	outb_p(PIC_ICW3_SLAVE_AT_IRQ2, PIC_MASTER_A0_1);
	outb_p(PIC_ICW4_8086_MODE | PIC_ICW4_AEOI_ENABLED | PIC_ICW4_MASTER, PIC_MASTER_A0_1);

	outb_p(PIC_ICW1_DEFAULT | PIC_ICW1_ICW4_NEEDED, PIC_SLAVE_A0_0);
	outb_p(slave_offset, PIC_SLAVE_A0_1);
	outb_p(PIC_ICW3_SLAVE_ID, PIC_MASTER_A0_1);
	outb_p(PIC_ICW4_8086_MODE | PIC_ICW4_AEOI_ENABLED | PIC_ICW4_MASTER, PIC_MASTER_A0_1);
	
	outb(0, PIC_MASTER_IMR);
	outb(0, PIC_SLAVE_IMR);
}

void shutdown_8259A(void)
{
	outb(0xff, PIC_SLAVE_IMR);
	outb(~(1 << PIC_CASCADE_IR), PIC_MASTER_IMR);
}

bool probe_8259A(void)
{
	uint8_t prev_master_mask;
	uint8_t prev_slave_mask;
	uint8_t probe_val;
	uint8_t response;
	bool ret;

	prev_master_mask = inb(PIC_MASTER_IMR);
	prev_slave_mask = inb(PIC_SLAVE_IMR);

	probe_val = ~(1 << PIC_CASCADE_IR);
	outb(0xff, PIC_SLAVE_IMR);
	outb(probe_val, PIC_MASTER_IMR);
	response = inb(PIC_MASTER_IMR);

	ret = (response == probe_val);
	outb(prev_slave_mask, PIC_SLAVE_IMR);
	outb(prev_master_mask, PIC_MASTER_IMR);
	return ret;
}