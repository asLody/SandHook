#include "shellcode_arm.h"

.global callback_addr_s
.global origin_addr_s


.data

FUNCTION_START(BP_SHELLCODE)
    push    {r0, r1, r2, r3}
    mrs     r0, cpsr
    str     r0, [sp, #0xC]
    str     r14, [sp, #8]
    add     r14, sp, #0x10
    str     r14, [sp, #4]
    pop     {r0}
    push    {r0-r12}
    mov     r0, sp
    ldr     r3, callback_addr_s
    blx     r3
    ldr     r0, [sp, #0x3C]
    msr     cpsr, r0
    ldmfd   sp!, {r0-r12}
    ldr     r14, [sp, #4]
    ldr     sp, [r13]
    ldr     pc, origin_addr_s
callback_addr_s:
    .long 0
origin_addr_s:
    .long 0
FUNCTION_END(BP_SHELLCODE)

.end