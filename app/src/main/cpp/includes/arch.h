//
// Created by 甘尧 on 2019/1/12.
//

#ifndef SANDHOOK_ARCH_H
#define SANDHOOK_ARCH_H

#endif //SANDHOOK_ARCH_H

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

}
