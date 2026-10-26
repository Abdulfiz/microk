#include "../drivers/uart.h"
#include "kernel.h"

int main(void)
{
    uart_puts("microk: M1 tick demo\n");
    systick_init(SYSTICK_1MS);

    uint32_t last = 0;
    for (;;) {
        uint32_t now = ticks_get();
        if (now >= last + 500) {      /* print every 500 ticks (~0.5 s) */
            last = now;
            uart_puts("ticks = ");
            uart_put_dec(now);
            uart_puts("\n");
        }
    }
}
