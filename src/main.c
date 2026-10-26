#include "../drivers/uart.h"
#include "kernel.h"

static uint32_t stack_p[128], stack_c[128];
static semaphore_t items;

/* Producer makes an item every 200 ms; consumer sleeps until one exists. */
static void producer(void)
{
    for (uint32_t n = 1; ; n++) {
        task_delay(200);
        uart_puts("produced item "); uart_put_dec(n); uart_puts("\n");
        sem_give(&items);
    }
}

static void consumer(void)
{
    for (uint32_t n = 1; ; n++) {
        sem_take(&items);                       /* blocks - uses no CPU while waiting */
        uart_puts("  consumed item "); uart_put_dec(n); uart_puts(" at tick ");
        uart_put_dec(ticks_get()); uart_puts("\n");
    }
}

int main(void)
{
    uart_puts("microk: M4 semaphore demo\n");
    sem_init(&items, 0);
    task_create(producer, stack_p, 128, "producer");
    task_create(consumer, stack_c, 128, "consumer");
    systick_init(SYSTICK_1MS);
    kernel_start();
}
