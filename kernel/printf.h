#ifndef _KERNEL_PRINTF_H
#define _KERNEL_PRINTF_H

int sprintf(char *buf, const char *fmt, ...);

void puts(const char *fmt);

int printf(const char *fmt, ...);

#endif /* _KERNEL_PRINTF_H */