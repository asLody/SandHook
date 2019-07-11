//
// Created by SwiftGan on 2019/4/15.
//

#pragma once

typedef size_t REG;

#define EXPORT  __attribute__ ((visibility ("default")))

extern "C"
EXPORT void* SandGetSym(const char* so, const char* sym);

extern "C"
EXPORT void* SandInlineHook(void* origin, void* replace);

extern "C"
EXPORT void* SandInlineHookSym(const char* so, const char* symb, void* replace);

extern "C"
EXPORT bool SandBreakpoint(void* origin, void (*callback)(REG[]));