#ifndef KERNEL_H
#define KERNEL_H
#include <stdint.h>

/* QEMU's lm3s6965 core clock is 12 MHz, so 12000 cycles = 1 ms */
#define SYSTICK_1MS 12000u

#define MAX_TASKS 8

/* ---- time ---- */
void     systick_init(uint32_t reload);   /* start the periodic tick timer */
uint32_t ticks_get(void);                 /* ticks since boot */

/* ---- tasks ---- */
typedef void (*task_fn)(void);

/* Create a task. 'stack' is memory you provide, 'words' its size in 32-bit
 * words (64 is enough for small tasks). Returns 0 on success, -1 if full. */
int  task_create(task_fn fn, uint32_t *stack, uint32_t words, const char *name);
void kernel_start(void);   /* start the first task; never returns */
void task_yield(void);     /* give up the CPU to the next task */
void task_delay(uint32_t ticks);   /* sleep for this many ticks (1 tick = 1 ms) */

#endif
