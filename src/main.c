#include "../drivers/uart.h"
#include "kernel.h"

static uint32_t stack_a[96], stack_b[96];

/* These tasks never yield: the timer interrupt preempts them. */
static void task_a(void)
{
    uart_puts("A started\n");
    for (;;) { uart_puts("A tick "); uart_put_dec(ticks_get()); uart_puts("\n"); task_delay(500); }
}

static void task_b(void)
{
    uart_puts("B started\n");
    for (;;) { uart_puts("  B tick "); uart_put_dec(ticks_get()); uart_puts("\n"); task_delay(300); }
}

int main(void)
{
    uart_puts("microk: M3 preemptive scheduler demo\n");
    task_create(task_a, stack_a, 96, "A");
    task_create(task_b, stack_b, 96, "B");
    systick_init(SYSTICK_1MS);
    kernel_start();
}
