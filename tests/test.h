#ifndef TEST_H
#define TEST_H
#include "../drivers/uart.h"
#include "semihost.h"

#define CHECK(cond) do { if (!(cond)) { \
    uart_puts("FAIL: " #cond " (line "); uart_put_dec(__LINE__); uart_puts(")\n"); \
    semihost_exit(1); } } while (0)
#define PASS(name)  uart_puts("PASS: " name "\n")
#define TESTS_DONE() do { uart_puts("ALL TESTS PASSED\n"); semihost_exit(0); } while (0)
#endif
