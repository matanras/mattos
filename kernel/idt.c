#include <stddef.h>
#include <idt.h>
#include <descriptor.h>
#include <kernel.h>
#include <segment.h>
#include <exceptions.h>

#define IDT_SIZE 			256

#define GATE_INTERRUPT 		0xe
#define GATE_TRAP			0xf

#define IDT_DESCRIPTOR_SIZE   8


union idt_descriptor_flags {
	struct {
		uint8_t gate_type : 4;
		uint8_t storage_segment : 1; /* set to 0 for interrupt and trap gates (see below). */
		uint8_t descriptor_privilege_level : 2; /* minimum priv lvl of calling descriptor */
		uint8_t present : 1; /* set 0 for unused interrupts */
	};
	uint8_t value;
};
static_assert_sizeof(union idt_descriptor_flags, sizeof(uint8_t));


struct idt_descriptor {
	uint16_t offset_1;
	union selector selector;
	uint8_t zero;
	union idt_descriptor_flags flags;
	uint16_t offset_2;
} __packed;
static_assert_sizeof(struct idt_descriptor, IDT_DESCRIPTOR_SIZE);


struct idt_data {
	void *isr;
	union selector selector;
	union idt_descriptor_flags flags;
};


#define IDT_DATA_ENTRY(_isr, priv, _table_type, descriptor_idx, gate)			\
	{																			\
		.isr = (void *)_isr,													\
		.selector = {															\
			.privilege_level = priv,											\
			.table_type = _table_type,											\
			.descriptor_index = descriptor_idx 									\
		},																		\
		.flags = {																\
			.gate_type = gate,													\
			.storage_segment = 0,												\
			.descriptor_privilege_level = priv,									\
			.present = 1														\
		}																		\
	}

#define FAULT_ENTRY(isr) \
	IDT_DATA_ENTRY(isr, DPL0, TABLE_TYPE_GDT, __KERNEL_CODE_SEG, GATE_TRAP)

static struct idt_data idt_data[] = {
	FAULT_ENTRY(divide_by_zero_fault)
};

struct idt_descriptor idt[IDT_SIZE];

struct idt_info_struct {
	uint16_t size; /* set to sizeof idt minus 1 */
	uintptr_t idt_ptr;
} __packed;
static_assert_sizeof(struct idt_info_struct, 6);

static struct idt_info_struct idt_info = {
	.size = 7,
	.idt_ptr = (uintptr_t)idt
};

void idt_set_entry(int entry, void *isr)
{
	struct idt_descriptor *desc;

	desc = &idt[entry];
	desc->offset_1 = (intptr_t)isr & 0x0000ffff;
	desc->offset_2 = ((intptr_t)isr & 0xffff0000) >> 16;
	desc->selector.descriptor_index = __KERNEL_CODE_SEG;
	desc->selector.table_type = TABLE_TYPE_GDT;
	desc->selector.privilege_level = DPL0;
	desc->zero = 0;
	desc->flags.gate_type = GATE_INTERRUPT;
	desc->flags.descriptor_privilege_level = DPL0;
	desc->flags.present = 1;
	desc->flags.storage_segment = 0;
}

void init_idt_descriptor(struct idt_descriptor *desc, const struct idt_data *data)
{
	desc->offset_1 = ((uintptr_t)data->isr) & 0x0000ffff;
	desc->zero = 0;
	desc->selector = data->selector;
	desc->flags = data->flags;
	desc->offset_2 = (((uintptr_t)data->isr) & 0xffff0000) >> 16;
}
#include <vga.h>

static inline void native_load_idt(const struct idt_info_struct *dtr)
{
	asm volatile("lidt %0"::"m" (*dtr));
}

static inline void native_store_idt(struct idt_info_struct *dtr)
{
	asm volatile("sidt %0":"=m" (*dtr));
}

void idt_load(void)
{
/* 	struct idt_data *iter;
	size_t idt_data_size = ARRAY_SIZE(idt_data);
	size_t i;
	struct idt_info_struct x;

	for (i = 0; i < idt_data_size; ++i) {
		init_idt_descriptor(&idt[i], &idt_data[i]);
	} */
	idt[0].offset_1 = (uintptr_t)divide_by_zero_fault & 0xffff;
	idt[0].selector.value = 0x8;
	idt[0].zero = 0;
	idt[0].flags.gate_type = GATE_TRAP;
	idt[0].flags.storage_segment = 0;
	idt[0].flags.descriptor_privilege_level = 0;
	idt[0].flags.present = 1;
	idt[0].offset_2 = ((uintptr_t)divide_by_zero_fault & 0xffff) >> 16;
	
	idt[8].offset_1 = (uintptr_t)double_fault & 0xffff;
	idt[8].selector.value = 0x8;
	idt[8].zero = 0;
	idt[8].flags.gate_type = GATE_TRAP;
	idt[8].flags.storage_segment = 0;
	idt[8].flags.descriptor_privilege_level = 0;
	idt[8].flags.present = 1;
	idt[8].offset_2 = ((uintptr_t)double_fault & 0xffff) >> 16;
	native_load_idt(&idt_info);
}