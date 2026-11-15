/* COM1 serial port output (16550 UART). Works without any firmware
 * services; in QEMU it appears wherever -serial points (stdio in make run). */
#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t v)
{
  __asm__ volatile ("outb %0, %1" : : "a"(v), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
  uint8_t v;
  __asm__ volatile ("inb %1, %0" : "=a"(v) : "Nd"(port));
  return v;
}

void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *s);
void serial_hex(uint64_t value);        /* 0x + 16 hex digits */
void serial_dec(uint64_t value);
void serial_pad(const char *s, int width);  /* left-aligned in width columns */

#endif /* SERIAL_H */
