/* Vector table and reset code for Cortex-M3. */
#include <stdint.h>

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;
extern int main(void);

void Default_Handler(void) { for (;;) { } }

/* "weak" = can be replaced by a real handler defined elsewhere */
void Reset_Handler(void);
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));

__attribute__((section(".isr_vector"), used))
void (*const vector_table[])(void) = {
    (void (*)(void))(&_estack),   /* 0: initial stack pointer */
    Reset_Handler,                /* 1 */
    NMI_Handler,                  /* 2 */
    HardFault_Handler,            /* 3 */
    MemManage_Handler,            /* 4 */
    BusFault_Handler,             /* 5 */
    UsageFault_Handler,           /* 6 */
    0, 0, 0, 0,                   /* 7-10: reserved */
    SVC_Handler,                  /* 11 */
    DebugMon_Handler,             /* 12 */
    0,                            /* 13: reserved */
    PendSV_Handler,               /* 14 */
    SysTick_Handler,              /* 15 */
};

void Reset_Handler(void)
{
    uint32_t *src = &_sidata, *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;   /* copy initialised data to RAM */
    for (dst = &_sbss; dst < &_ebss; ) *dst++ = 0;   /* zero uninitialised data */
    main();
    for (;;) { }
}
