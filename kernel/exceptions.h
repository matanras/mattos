#pragma once

#include <interrupt.h>

__interrupt void divide_by_zero_fault(struct interrupt_frame *ir_frame);

__interrupt void double_fault(struct interrupt_frame *ir_frame);