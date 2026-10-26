/* Semaphore hand-off works, and the mutex prevents lost updates. */
#include "test.h"
#include "kernel.h"

static uint32_t st_chk[128], st_prod[128], st_cons[128], st_w1[128], st_w2[128], st_w3[128];

static semaphore_t items;
static volatile int produced, consumed;

static void producer(void) { for (int i = 0; i < 20; i++) { task_delay(2); produced++; sem_give(&items); } for (;;) task_delay(1000); }
static void consumer(void) { for (int i = 0; i < 20; i++) { sem_take(&items); consumed++; } for (;;) task_delay(1000); }

static mutex_t lock;
static volatile int shared, finished;

/* Read, yield, write: without the mutex, other workers would sneak in
 * during the yield and updates would be lost. */
static void worker(void)
{
    for (int i = 0; i < 50; i++) {
        mutex_lock(&lock);
        int tmp = shared;
        task_yield();
        shared = tmp + 1;
        mutex_unlock(&lock);
    }
    mutex_lock(&lock); finished++; mutex_unlock(&lock);
    for (;;) task_delay(1000);
}

static void checker(void)
{
    task_delay(300);
    CHECK(produced == 20);
    CHECK(consumed == 20);
    PASS("semaphore: 20 items produced and consumed");
    CHECK(finished == 3);
    CHECK(shared == 150);
    PASS("mutex: 3 workers x 50 increments = 150, none lost");
    TESTS_DONE();
}

int main(void)
{
    sem_init(&items, 0);
    mutex_init(&lock);
    task_create(checker,  st_chk,  128, "checker");
    task_create(producer, st_prod, 128, "producer");
    task_create(consumer, st_cons, 128, "consumer");
    task_create(worker,   st_w1,   128, "w1");
    task_create(worker,   st_w2,   128, "w2");
    task_create(worker,   st_w3,   128, "w3");
    systick_init(SYSTICK_1MS);
    kernel_start();
}
