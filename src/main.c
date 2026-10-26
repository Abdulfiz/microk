#include "../drivers/uart.h"
#include "kernel.h"

static uint32_t stack_a[64], stack_b[64], stack_c[64];

static void task_a(void) { for (int i = 0; i < 5; i++) { uart_puts("A running\n"); task_yield(); } for (;;) task_yield(); }
static void task_b(void) { for (int i = 0; i < 5; i++) { uart_puts("  B running\n"); task_yield(); } for (;;) task_yield(); }
static void task_c(void) { for (int i = 0; i < 5; i++) { uart_puts("    C running\n"); task_yield(); } for (;;) task_yield(); }

int main(void)
{
    uart_puts("microk: M2 cooperative switching demo\n");
    task_create(task_a, stack_a, 64, "A");
    task_create(task_b, stack_b, 64, "B");
    task_create(task_c, stack_c, 64, "C");
    kernel_start();
}
