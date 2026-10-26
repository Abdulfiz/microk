#ifndef SEMIHOST_H
#define SEMIHOST_H
/* Ask QEMU to quit. code 0 = success, anything else = failure. */
static inline void semihost_exit(int code)
{
    register unsigned r0 __asm__("r0") = 0x18;                       /* SYS_EXIT */
    register unsigned r1 __asm__("r1") = code ? 0x20023 : 0x20026;   /* reason */
    __asm__ volatile("bkpt 0xAB" : : "r"(r0), "r"(r1) : "memory");
    for (;;) { }
}
#endif
