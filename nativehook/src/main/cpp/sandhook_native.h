//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_SANDHOOK_NATIVE_H
#define SANDHOOK_SANDHOOK_NATIVE_H

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

#endif //SANDHOOK_SANDHOOK_NATIVE_H
