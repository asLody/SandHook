//
// Created by SwiftGan on 2019/1/17.
//

#ifndef SANDHOOK_TRAMPOLINE_H
#define SANDHOOK_TRAMPOLINE_H

#include <cstdint>
#include <string.h>
#include "../includes/arch.h"
#include "./arch/base.h"
#include "stdlib.h"

#define Code unsigned char *

#if defined(__i386__)
#define SIZE_REPLACEMENT_HOOK_TRAMPOLINE 4 * 9
#define OFFSET_REPLACEMENT_ADDR_ART_METHOD 4 * 5
#define OFFSET_REPLACEMENT_OFFSET_ENTRY_CODE 4 * 7

#define SIZE_DIRECT_JUMP_TRAMPOLINE 4 * 4
#define OFFSET_JUMP_ADDR_TARGET 4 * 2

#define SIZE_INLINE_HOOK_TRAMPOLINE 4 * 24
#define OFFSET_INLINE_ADDR_ORIGIN_METHOD 4 * 10
#define OFFSET_INLINE_ORIGIN_CODE 4 * 12
#define OFFSET_INLINE_OFFSET_ENTRY_CODE 4 * 20
#define OFFSET_INLINE_ADDR_HOOK_METHOD 4 * 22
#elif defined(__x86_64__)
#define SIZE_REPLACEMENT_HOOK_TRAMPOLINE 4 * 9
#define OFFSET_REPLACEMENT_ADDR_ART_METHOD 4 * 5
#define OFFSET_REPLACEMENT_OFFSET_ENTRY_CODE 4 * 7

#define SIZE_DIRECT_JUMP_TRAMPOLINE 4 * 4
#define OFFSET_JUMP_ADDR_TARGET 4 * 2

#define SIZE_INLINE_HOOK_TRAMPOLINE 4 * 24
#define OFFSET_INLINE_ADDR_ORIGIN_METHOD 4 * 10
#define OFFSET_INLINE_ORIGIN_CODE 4 * 12
#define OFFSET_INLINE_OFFSET_ENTRY_CODE 4 * 20
#define OFFSET_INLINE_ADDR_HOOK_METHOD 4 * 22
#elif defined(__arm__)
#define SIZE_REPLACEMENT_HOOK_TRAMPOLINE 4 * 9
#define OFFSET_REPLACEMENT_ADDR_ART_METHOD 4 * 5
#define OFFSET_REPLACEMENT_OFFSET_ENTRY_CODE 4 * 7

#define SIZE_DIRECT_JUMP_TRAMPOLINE 4 * 4
#define OFFSET_JUMP_ADDR_TARGET 4 * 2

#define SIZE_INLINE_HOOK_TRAMPOLINE 4 * 24
#define OFFSET_INLINE_ADDR_ORIGIN_METHOD 4 * 10
#define OFFSET_INLINE_ORIGIN_CODE 4 * 12
#define OFFSET_INLINE_OFFSET_ENTRY_CODE 4 * 20
#define OFFSET_INLINE_ADDR_HOOK_METHOD 4 * 22
#elif defined(__aarch64__)
#define SIZE_REPLACEMENT_HOOK_TRAMPOLINE 4 * 9
#define OFFSET_REPLACEMENT_ADDR_ART_METHOD 4 * 5
#define OFFSET_REPLACEMENT_OFFSET_ENTRY_CODE 4 * 7

#define SIZE_DIRECT_JUMP_TRAMPOLINE 4 * 4
#define OFFSET_JUMP_ADDR_TARGET 4 * 2

#define SIZE_INLINE_HOOK_TRAMPOLINE 4 * 24
#define OFFSET_INLINE_ADDR_ORIGIN_METHOD 4 * 10
#define OFFSET_INLINE_ORIGIN_CODE 4 * 12
#define OFFSET_INLINE_OFFSET_ENTRY_CODE 4 * 20
#define OFFSET_INLINE_ADDR_HOOK_METHOD 4 * 22
#else
#endif

extern "C" void DIRECT_JUMP_TRAMPOLINE();
extern "C" void INLINE_HOOK_TRAMPOLINE();
extern "C" void REPLACEMENT_HOOK_TRAMPOLINE();

namespace SandHook {

    class Trampoline {
    public:

        Trampoline() = default;

        virtual void init() {
            codeLen = codeLength();
            tempCode = templateCode();
        }
        const void setExecuteSpace(Code start) {
            code = start;
            memcpy(code, tempCode, codeLen);
        }
        const void codeCopy(Code src, Size targetOffset, Size len) {
            memcpy(code + targetOffset, src, len);
        }
        const void clone(Code dest) {
            memcpy(dest, code, codeLen);
        }
        const Code getCode() {
            return code;
        }
        const Size getCodeLen() {
            return codeLen;
        }
    protected:
        virtual Size codeLength() = 0;
        virtual Code templateCode() = 0;
    private:
        Code code;
        Code tempCode;
        Size codeLen;
    };

}


#endif //SANDHOOK_TRAMPOLINE_H
