#include "serial.h"

#define COM1 0x3F8

void serial_init(void)
{
  outb(COM1 + 1, 0x00);   /* disable interrupts */
  outb(COM1 + 3, 0x80);   /* enable DLAB to set the baud rate divisor */
  outb(COM1 + 0, 0x01);   /* divisor 1 = 115200 baud (low byte) */
  outb(COM1 + 1, 0x00);   /*                         (high byte) */
  outb(COM1 + 3, 0x03);   /* 8 bits, no parity, one stop bit, DLAB off */
  outb(COM1 + 2, 0xC7);   /* enable and clear FIFOs */
  outb(COM1 + 4, 0x03);   /* DTR + RTS */
}

void serial_putc(char c)
{
  if (c == '\n')
    serial_putc('\r');
  while ((inb(COM1 + 5) & 0x20) == 0)   /* wait until transmit buffer is empty */
    ;
  outb(COM1, (uint8_t)c);
}

void serial_puts(const char *s)
{
  while (*s)
    serial_putc(*s++);
}

void serial_hex(uint64_t value)
{
  serial_puts("0x");
  for (int i = 15; i >= 0; i--) {
    uint8_t nibble = (value >> (i * 4)) & 0xF;
    serial_putc(nibble < 10 ? '0' + nibble : 'A' + (nibble - 10));
  }
}

void serial_dec(uint64_t value)
{
  char buf[21];
  int i = 20;
  buf[i] = 0;
  do {
    buf[--i] = '0' + (value % 10);
    value /= 10;
  } while (value != 0);
  serial_puts(&buf[i]);
}

void serial_pad(const char *s, int width)
{
  while (*s) {
    serial_putc(*s++);
    width--;
  }
  while (width-- > 0)
    serial_putc(' ');
}
