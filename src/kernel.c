#include "kernel.h"

/* Cortex-M SysTick timer registers */
#define SYST_CSR (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR (*(volatile uint32_t *)0xE000E018)

static volatile uint32_t tick_count;

void systick_init(uint32_t reload)
{
    SYST_RVR = reload - 1;   /* counts down from here, then fires */
    SYST_CVR = 0;            /* clear the current value */
    SYST_CSR = 7;            /* enable counter + interrupt + core clock */
}

uint32_t ticks_get(void) { return tick_count; }

void SysTick_Handler(void)   /* runs every time the timer reaches zero */
{
    tick_count++;
}
