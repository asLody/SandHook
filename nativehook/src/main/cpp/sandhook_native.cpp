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
    return reinterpret_cast<void *>(elfImg.getSymbAddress(symb));
}

extern "C"
EXPORT void* SandInlineHook(void* origin, void* replace) {
    return InlineHook::instance->inlineHook(origin, replace);
}

extern "C"
EXPORT void* SandInlineHookSym(const char* so, const char* symb, void* replace) {
    ElfImg elfImg(so);
    void* origin = reinterpret_cast<void *>(elfImg.getSymbAddress(symb));

    if (origin == nullptr)
        return nullptr;
    return InlineHook::instance->inlineHook(origin, replace);
}

extern "C"
EXPORT bool SandBreakpoint(void* origin, void (*callback)(REG[])) {
    return InlineHook::instance->breakPoint(origin, callback);
}