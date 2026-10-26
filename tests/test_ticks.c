/* The 1 ms timer counts upward. */
#include "test.h"
#include "kernel.h"

int main(void)
{
    systick_init(SYSTICK_1MS);
    CHECK(ticks_get() < 5);
    while (ticks_get() < 50) { }
    PASS("tick counter reaches 50");
    uint32_t a = ticks_get();
    uint32_t b;
    while ((b = ticks_get()) == a) { }   /* read once, so the value cannot change under us */
    CHECK(b == a + 1);
    PASS("tick counter increases by exactly 1");
    TESTS_DONE();
}
