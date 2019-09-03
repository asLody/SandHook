//
// Created by SwiftGan on 2019/4/15.
//

#include <jni.h>
#include <cassert>
#include <cstdlib>
#include "sandhook_native.h"
#include "hook.h"
#include "elf.h"

using namespace SandHook::Hook;
using namespace SandHook::Elf;

extern "C"
EXPORT void* SandGetModuleBase(const char* so) {
    return ElfImg::GetModuleBase(so);
}

extern "C"
EXPORT void* SandGetSym(const char* so, const char* symb) {
    ElfImg elfImg(so);
    return reinterpret_cast<void *>(elfImg.GetSymAddress(symb));
}

extern "C"
EXPORT void* SandInlineHook(void* origin, void* replace) {
    return InlineHook::instance->Hook(origin, replace);
}

extern "C"
EXPORT void* SandInlineHookSym(const char* so, const char* symb, void* replace) {
    ElfImg elfImg(so);
    void* origin = reinterpret_cast<void *>(elfImg.GetSymAddress(symb));

    if (origin == nullptr)
        return nullptr;
    return InlineHook::instance->Hook(origin, replace);
}

extern "C"
EXPORT void* SandSingleInstHook(void* origin, void* replace) {
    return InlineHook::instance->SingleInstHook(origin, replace);
}

extern "C"
EXPORT void* SandSingleInstHookSym(const char* so, const char* symb, void* replace) {
    ElfImg elfImg(so);
    void* origin = reinterpret_cast<void *>(elfImg.GetSymAddress(symb));

    if (origin == nullptr)
        return nullptr;
    return InlineHook::instance->SingleInstHook(origin, replace);
}

extern "C"
EXPORT bool SandBreakPoint(void *origin, void (*callback)(REG[])) {
    return InlineHook::instance->BreakPoint(origin, callback);
}

extern "C"
EXPORT bool SandSingleInstBreakPoint(void *origin, BreakCallback(callback)) {
    return InlineHook::instance->SingleBreakPoint(origin, callback);
}

#if defined(__aarch64__)

fpsimd_context* GetSimdContext(sigcontext *mcontext) {
    size_t size = 0;
    do {
        fpsimd_context *fp = reinterpret_cast<fpsimd_context *>(&mcontext->__reserved[size]);
        if (fp->head.magic == FPSIMD_MAGIC) {
            assert(fp->head.size >= sizeof(fpsimd_context));
            assert(size + fp->head.size <= sizeof(mcontext->__reserved));
            return fp;
        }
        if (fp->head.size == 0) {
            break;
        }
        size += fp->head.size;
    } while (size + sizeof(fpsimd_context) <= sizeof(mcontext->__reserved));
    abort();
    return nullptr;
}

#endif