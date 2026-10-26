/* Minimal UART0 driver for the QEMU lm3s6965evb board. */
#include "uart.h"

#define UART0_BASE 0x4000C000u
#define UART0_DR   (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_FR   (*(volatile uint32_t *)(UART0_BASE + 0x18))
#define FR_TXFF    (1u << 5)   /* transmit FIFO full */

void uart_putc(char c)
{
    while (UART0_FR & FR_TXFF) { }
    UART0_DR = (uint32_t)c;
}

void uart_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

void uart_put_dec(uint32_t v)
{
    char buf[11];
    int i = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v) {                       /* digits come out in reverse order */
        uint32_t q = v / 10;
        buf[i++] = (char)('0' + (v - q * 10));
        v = q;
    }
    while (i) uart_putc(buf[--i]);
}

void uart_put_hex(uint32_t v)
{
    uart_puts("0x");
    for (int shift = 28; shift >= 0; shift -= 4)
        uart_putc("0123456789abcdef"[(v >> shift) & 0xF]);
}
