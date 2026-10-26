/* Two tasks that never yield still both make progress (the timer preempts them). */
#include "test.h"
#include "kernel.h"

static uint32_t st_main[128], st_a[128], st_b[128];
static volatile uint32_t count_a, count_b;

static void spin_a(void) { for (;;) count_a++; }
static void spin_b(void) { for (;;) count_b++; }

static void checker(void)
{
    task_delay(100);
    CHECK(count_a > 0);
    CHECK(count_b > 0);
    PASS("both busy-loop tasks ran without yielding");
    uint32_t big = count_a > count_b ? count_a : count_b;
    uint32_t small = count_a > count_b ? count_b : count_a;
    CHECK(small > big / 4);               /* roughly fair share of the CPU */
    PASS("CPU time is shared roughly fairly");
    TESTS_DONE();
}

int main(void)
{
    task_create(checker, st_main, 128, "checker");
    task_create(spin_a,  st_a, 128, "spinA");
    task_create(spin_b,  st_b, 128, "spinB");
    systick_init(SYSTICK_1MS);
    kernel_start();
}
