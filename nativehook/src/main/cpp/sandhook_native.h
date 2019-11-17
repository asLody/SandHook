//
// Created by SwiftGan on 2019/4/15.
//

#pragma once

#include <signal.h>

typedef size_t REG;

#define EXPORT  __attribute__ ((visibility ("default")))

#define BreakCallback(callback) bool(*callback)(sigcontext*, void*)

extern "C"
EXPORT void* SandGetModuleBase(const char* so);

extern "C"
EXPORT void* SandGetSym(const char* so, const char* sym);

extern "C"
EXPORT void* SandInlineHook(void* origin, void* replace);

extern "C"
EXPORT void* SandInlineHookSym(const char* so, const char* symb, void* replace);

extern "C"
EXPORT void* SandSingleInstHook(void* origin, void* replace);

extern "C"
EXPORT void* SandSingleInstHookSym(const char* so, const char* symb, void* replace);

extern "C"
EXPORT bool SandBreakPoint(void *origin, void (*callback)(REG[]));

extern "C"
EXPORT bool SandSingleInstBreakPoint(void *origin, BreakCallback(callback));

#if defined(__aarch64__)

#include <asm/sigcontext.h>
extern "C"
EXPORT fpsimd_context* GetSimdContext(sigcontext *mcontext);

#endif