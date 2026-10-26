#include "kernel.h"

/* ---------- Cortex-M system registers ---------- */
#define SYST_CSR (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR (*(volatile uint32_t *)0xE000E018)
#define SCB_ICSR (*(volatile uint32_t *)0xE000ED04)
#define SCB_SHPR3 (*(volatile uint32_t *)0xE000ED20)
#define ICSR_PENDSVSET (1u << 28)

/* ---------- task control block ---------- */
typedef enum { TASK_READY = 0, TASK_DELAYED } task_state_t;

typedef struct tcb {
    uint32_t  *sp;          /* saved stack pointer. MUST be the first field:
                               the assembly below reads it at offset 0.   */
    const char *name;
    task_state_t state;
    uint32_t   wake_tick;   /* when a DELAYED task becomes READY again */
} tcb_t;

/* tasks[0] is the idle task: it runs only when nothing else can. */
static tcb_t    tasks[MAX_TASKS + 1];
static int      task_count;          /* includes the idle task once created */
static uint32_t idle_stack[48];
tcb_t          *current_task;        /* not static: the assembly refers to this name */

static volatile uint32_t tick_count;
static volatile int      kernel_running;   /* set once kernel_start() runs */

/* ---------- time ---------- */
void systick_init(uint32_t reload)
{
    SYST_RVR = reload - 1;
    SYST_CVR = 0;
    SYST_CSR = 7;
}

uint32_t ticks_get(void) { return tick_count; }

void SysTick_Handler(void)
{
    tick_count++;
    if (kernel_running)          /* before kernel_start() there is nothing to switch */
        SCB_ICSR = ICSR_PENDSVSET;   /* every tick, ask the scheduler to run */
}

/* ---------- task creation ---------- */
static void task_exit(void)   /* a task that returns from its function lands here */
{
    for (;;) task_yield();
}

static void idle_task(void)
{
    for (;;) __asm volatile("wfi");   /* sleep until the next interrupt */
}

static int add_task(task_fn fn, uint32_t *stack, uint32_t words, const char *name);

int task_create(task_fn fn, uint32_t *stack, uint32_t words, const char *name)
{
    if (task_count == 0)   /* first call: create the idle task in slot 0 */
        add_task(idle_task, idle_stack, 48, "idle");
    if (task_count >= MAX_TASKS + 1) return -1;
    return add_task(fn, stack, words, name);
}

static int add_task(task_fn fn, uint32_t *stack, uint32_t words, const char *name)
{

    /* top of stack, aligned to 8 bytes (an ARM requirement) */
    uint32_t *sp = (uint32_t *)(((uintptr_t)(stack + words)) & ~7u);

    /* Build a fake "exception frame": the CPU pops this when it first
     * switches to the task, which makes the task start running at fn(). */
    *(--sp) = 0x01000000u;              /* xPSR  (Thumb bit must be set) */
    *(--sp) = ((uint32_t)fn) & ~1u;     /* PC    = where the task starts */
    *(--sp) = (uint32_t)task_exit;      /* LR    = where it goes if it returns */
    *(--sp) = 0;                        /* R12 */
    *(--sp) = 0;                        /* R3  */
    *(--sp) = 0;                        /* R2  */
    *(--sp) = 0;                        /* R1  */
    *(--sp) = 0;                        /* R0  */
    /* R4-R11 are saved/restored by software in PendSV */
    for (int i = 0; i < 8; i++) *(--sp) = 0;

    tcb_t *t = &tasks[task_count++];
    t->sp   = sp;
    t->name = name;
    t->state = TASK_READY;
    t->wake_tick = 0;
    return 0;
}

/* ---------- scheduler ---------- */
/* Called from PendSV with the old task already saved. Picks the next task
 * (simple round-robin) and stores it in current_task. */
void scheduler_pick(void)
{
    /* 1. wake any delayed task whose time has come
     *    (the subtraction form stays correct when the tick counter wraps) */
    for (int i = 1; i < task_count; i++)
        if (tasks[i].state == TASK_DELAYED &&
            (int32_t)(tick_count - tasks[i].wake_tick) >= 0)
            tasks[i].state = TASK_READY;

    /* 2. round-robin: look for the next READY task after the current one.
     *    User tasks live in slots 1..task_count-1 (slot 0 is idle). */
    int idx    = (int)(current_task - tasks);
    int n_user = task_count - 1;
    for (int n = 1; n <= n_user; n++) {
        int cand = (idx == 0) ? n : 1 + ((idx - 1 + n) % n_user);
        if (tasks[cand].state == TASK_READY) {
            current_task = &tasks[cand];
            return;
        }
    }
    current_task = &tasks[0];   /* nothing ready: run idle */
}

/* ---------- context switching (assembly) ---------- */
void task_delay(uint32_t ticks)
{
    __asm volatile("cpsid i");
    current_task->wake_tick = tick_count + ticks;
    current_task->state = TASK_DELAYED;
    __asm volatile("cpsie i");
    task_yield();
}

void task_yield(void)
{
    SCB_ICSR = ICSR_PENDSVSET;          /* ask for a context switch */
    __asm volatile("dsb\n isb\n");
}

/* PendSV: save the running task, pick another, restore it. */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile(
        "cpsid i\n"
        "mrs   r0, psp\n"               /* r0 = running task's stack pointer */
        "stmdb r0!, {r4-r11}\n"         /* push R4-R11 onto its stack */
        "ldr   r1, =current_task\n"
        "ldr   r2, [r1]\n"
        "str   r0, [r2]\n"              /* current_task->sp = r0 */
        "push  {r1, lr}\n"
        "bl    scheduler_pick\n"        /* current_task = next task */
        "pop   {r1, lr}\n"
        "ldr   r2, [r1]\n"
        "ldr   r0, [r2]\n"              /* r0 = next task's saved sp */
        "ldmia r0!, {r4-r11}\n"         /* pop its R4-R11 */
        "msr   psp, r0\n"
        "cpsie i\n"
        "bx    lr\n"                    /* CPU pops the rest and jumps to the task */
        ".ltorg\n");
}

/* SVC: used once, to launch the very first task. */
__attribute__((naked)) void SVC_Handler(void)
{
    __asm volatile(
        "ldr   r0, =current_task\n"
        "ldr   r0, [r0]\n"
        "ldr   r0, [r0]\n"              /* r0 = first task's saved sp */
        "ldmia r0!, {r4-r11}\n"
        "msr   psp, r0\n"
        "ldr   lr, =0xFFFFFFFD\n"       /* "return to thread mode, use PSP" */
        "bx    lr\n"
        ".ltorg\n");
}

void kernel_start(void)
{
    SCB_SHPR3 |= (0xFFu << 16) | (0xFFu << 24);   /* PendSV + SysTick = lowest priority */
    current_task = &tasks[task_count > 1 ? 1 : 0];   /* start with the first user task */
    kernel_running = 1;
    __asm volatile("cpsie i\n svc 0\n");          /* launch the first task */
    for (;;) { }
}
