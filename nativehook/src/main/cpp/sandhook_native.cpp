//
// Created by SwiftGan on 2019/4/15.
//

#include <jni.h>
#include <cassert>
#include <cstdlib>
#include "sandhook_native.h"
#include "hook.h"
#include "elf.h"
#include <sys/mman.h>

using namespace SandHook::Hook;
using namespace SandHook::Elf;

#define _uintval(p)               reinterpret_cast<uintptr_t>(p)
#define _ptr(p)                   reinterpret_cast<void *>(p)
#define _align_up(x, n)           (((x) + ((n) - 1)) & ~((n) - 1))
#define _align_down(x, n)         ((x) & -(n))
#define _page_size                4096
#define _page_align(n)            _align_up(static_cast<uintptr_t>(n), _page_size)
#define _ptr_align(x)             _ptr(_align_down(reinterpret_cast<uintptr_t>(x), _page_size))
#define _make_rwx(p, n)           ::mprotect(_ptr_align(p), \
                                              _page_align(_uintval(p) + n) != _page_align(_uintval(p)) ? _page_align(n) + _page_size : _page_align(n), \
                                              PROT_READ | PROT_WRITE | PROT_EXEC)


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
    _make_rwx(origin, _page_size);
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
