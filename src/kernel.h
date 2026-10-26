#ifndef KERNEL_H
#define KERNEL_H
#include <stdint.h>

/* QEMU's lm3s6965 core clock is 12 MHz, so 12000 cycles = 1 ms */
#define SYSTICK_1MS 12000u

/* ---- time ---- */
void     systick_init(uint32_t reload);   /* start the periodic tick timer */
uint32_t ticks_get(void);                 /* ticks since boot */

#endif
