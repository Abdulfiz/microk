#include "../drivers/uart.h"

int main(void)
{
    uart_puts("microk: hello from Cortex-M3!\n");
    for (;;) { }
}
