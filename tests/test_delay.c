/* task_delay() sleeps for the right time and tasks wake in the right order. */
#include "test.h"
#include "kernel.h"

static uint32_t st_main[128], st_a[128], st_b[128];
static volatile int order[2], n_order;

static void sleeper_a(void) { task_delay(30); order[n_order++] = 'A'; for (;;) task_delay(1000); }
static void sleeper_b(void) { task_delay(10); order[n_order++] = 'B'; for (;;) task_delay(1000); }

static void checker(void)
{
    uint32_t t0 = ticks_get();
    task_delay(20);
    uint32_t dt = ticks_get() - t0;
    CHECK(dt >= 20 && dt <= 22);
    PASS("task_delay(20) sleeps 20-22 ticks");

    task_delay(30);                       /* total ~50 ticks: both sleepers done */
    CHECK(n_order == 2);
    CHECK(order[0] == 'B' && order[1] == 'A');
    PASS("shorter sleeper wakes first");
    TESTS_DONE();
}

int main(void)
{
    task_create(checker,   st_main, 128, "checker");
    task_create(sleeper_a, st_a, 128, "A");
    task_create(sleeper_b, st_b, 128, "B");
    systick_init(SYSTICK_1MS);
    kernel_start();
}
