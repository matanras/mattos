#include "io.h"

void memcpy_io(volatile void *dst, const volatile void *src, size_t size)
{
	volatile uint32_t *dst32, *src32;
	volatile uint16_t *dst16, *src16;

	if (size == 0)
		return;

	if (size == 1)
		*(volatile uint8_t *)dst = *(volatile uint8_t *)src;
	else if (size == 2)
		*(volatile uint16_t *)dst = *(volatile uint16_t *)src;
	else {
		src32 = (volatile uint32_t *)src;
		dst32 = (volatile uint32_t *)dst;
		
		while (size >= 4) {
			*dst32++ = *src32++;
			size -= sizeof(uint32_t);
		}

		if (size >= 2) {
			src16 = (volatile uint16_t *)src32;
			dst16 = (volatile uint16_t *)dst32;
			*dst16++ = *src16++;
			size -= sizeof(uint16_t);
		}

		if (size == 1)
			*(volatile uint8_t *)dst16 = *(volatile uint8_t *)src16;
	}
}

void memset_io(volatile void *dst, int val, size_t size)
{
	volatile uint8_t *iter = (volatile uint8_t *)dst;

	while (size > 0) {
		*iter++ = (volatile uint8_t)(val & 0xff);
		--size;
	}
}