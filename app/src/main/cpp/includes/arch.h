//
// Created by 甘尧 on 2019/1/12.
//

#ifndef SANDHOOK_ARCH_H
#define SANDHOOK_ARCH_H

namespace SandHook {

#define BYTE_POINT sizeof(void*)

//32bit
#if defined(__i386__) || defined(__arm__)
#define ADDR(addr) *((uint32_t *) addr)
#define ALIGN_ADDR(x) ((v+4-1) - ((v+4-1)&3))
#define Size uint32_t
//64bit
#elif defined(__aarch64__) || defined(__x86_64__)
#define ADDR(addr) *((uint64_t *) addr)
#define ALIGN_ADDR(x) ((v+8-1) - ((v+8-1)&7))
#define Size uint64_t
#else
#endif

#if defined(__arm__)
static void clearCacheArm32(char* begin, char *end)
{
    const int syscall = 0xf0002;
    __asm __volatile (
        "mov     r0, %0\n"
        "mov     r1, %1\n"
        "mov     r3, %2\n"
        "mov     r2, #0x0\n"
        "svc     0x00000000\n"
        :
        :    "r" (begin), "r" (end), "r" (syscall)
        :    "r0", "r1", "r3"
        );
}
#endif


}

#endif //SANDHOOK_ARCH_H