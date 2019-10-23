#ifndef _IO_H
#define _IO_H

#include <stdint.h>

/*
 * This file contains definitions for x86 IO operatins.
 */


#define BUILDIO(bwl, bw, type) 									\
static inline void out##bwl(type value, uint32_t port)			\
{																\
	asm volatile("out" #bwl " %" #bw "[value], %w[port]"		\
				 :												\
				 : [value] "a"(value), [port] "Nd"(port));		\
}																\
																\
static inline type in##bwl(uint32_t port)						\
{																\
	type ret;													\
																\
	asm volatile("in" #bwl " %w[port], %" #bw "[ret]"			\
				 : [ret] "=a"(ret)								\
				 : [port] "Nd"(port));							\
	return ret;													\
}

BUILDIO(b, b, uint8_t)
BUILDIO(w, w, uint16_t)
BUILDIO(l, , uint32_t)

#endif /* _IO_H */