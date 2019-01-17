//
// Created by SwiftGan on 2019/1/17.
//

#ifndef SANDHOOK_TRAMPOLINE_H
#define SANDHOOK_TRAMPOLINE_H

extern void hook_trampoline();

#if defined(__i386__)
#define SIZE_TRAMPOLINE = 4 * 7
#define OFFSET_ADDR_ART_METHOD 4 * 3
#define OFFSET_ADDR_ENTRY_CODE 4 * 5
#elif defined(__x86_64__)
#define SIZE_TRAMPOLINE = 4 * 7
#define OFFSET_ADDR_ART_METHOD 4 * 3
#define OFFSET_ADDR_ENTRY_CODE 4 * 5
#elif defined(__arm__)
#define SIZE_TRAMPOLINE = 4 * 7
#define OFFSET_ADDR_ART_METHOD 4 * 3
#define OFFSET_ADDR_ENTRY_CODE 4 * 5
#elif defined(__aarch64__)
#define SIZE_TRAMPOLINE = 4 * 7
#define OFFSET_ADDR_ART_METHOD 4 * 3
#define OFFSET_ADDR_ENTRY_CODE 4 * 5
#endif


#endif //SANDHOOK_TRAMPOLINE_H
