#pragma once

#include <stdint.h>
#include <kernel.h>

#define DPL0 0 /* ring zero, most privileged */
#define DPL3 3 /* ring 3, least privileged */

#define TABLE_TYPE_GDT 0
#define TABLE_TYPE_LDT 1

union selector{
	struct {
        uint16_t privilege_level : 2;
        uint16_t table_type : 1; /* 0 for gdt, 1 for ldt */
        uint16_t descriptor_index : 13;
    };
    uint16_t value;
};
static_assert_sizeof(union selector, sizeof(uint16_t));
