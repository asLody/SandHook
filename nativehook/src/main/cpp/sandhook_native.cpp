//
// Created by SwiftGan on 2019/4/15.
//

#include <jni.h>
#include "sandhook_native.h"
#include "hook.h"
#include "elf.h"

using namespace SandHook::Hook;
using namespace SandHook::Elf;

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
EXPORT bool SandBreakpoint(void* origin, void (*callback)(REG[])) {
    return InlineHook::instance->BreakPoint(origin, callback);
}