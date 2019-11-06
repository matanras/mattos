#ifndef _IO_H
#define _IO_H

#include <stdint.h>
#include <stddef.h>

/*
 * This file contains definitions for x86 IO operatins.
 */

static inline void io_delay(void)
{
	asm volatile ("outb %al, $0x80");
}

#define BUILDIO(bwl, bw, type) 									\
static inline void out##bwl(type value, uint16_t port)			\
{																\
	asm volatile("out" #bwl " %" #bw "[value], %w[port]"		\
				 :												\
				 : [value] "a"(value), [port] "Nd"(port));		\
}																\
																\
static inline void out##bwl##_p(type value, uint16_t port)		\
{																\
	asm volatile("out" #bwl " %" #bw "[value], %w[port]"		\
				 :												\
				 : [value] "a"(value), [port] "Nd"(port));		\
	io_delay();													\
}																\
																\
static inline type in##bwl(uint16_t port)						\
{																\
	type ret;													\
																\
	asm volatile("in" #bwl " %w[port], %" #bw "[ret]"			\
				 : [ret] "=a"(ret)								\
				 : [port] "Nd"(port));							\
	return ret;													\
}																\
																\
static inline type in##bwl##_p(uint16_t port)					\
{																\
	type ret;													\
																\
	asm volatile("in" #bwl " %w[port], %" #bw "[ret]"			\
				 : [ret] "=a"(ret)								\
				 : [port] "Nd"(port));							\
	io_delay();													\
	return ret;													\
}

BUILDIO(b, b, uint8_t)
BUILDIO(w, w, uint16_t)
BUILDIO(l, , uint32_t)

/**
 * Copy IO memory from one IO address to another.
 */
void memcpy_io(volatile void *dst, const volatile void *src, size_t size);
void memset_io(volatile void *dst, int val, size_t size);

#endif /* _IO_H */