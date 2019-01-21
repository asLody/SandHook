//
// Created by swift on 2019/1/21.
//

#ifndef SANDHOOK_HIDE_API_H
#define SANDHOOK_HIDE_API_H

#include <jni.h>
#include "fake_dlfcn.h"
#include "dlfcn.h"


void* (*jitLoad)(bool*) = nullptr;
void* jitCompilerHandle = nullptr;
bool (*jitCompileMethod)(void*, void*, void*, bool) = nullptr;

void initHideApi(JNIEnv* env, int SDK_VERSION) {
    if (SDK_VERSION >= 24) {
        void *handle;
        void *jit_lib;
        if (sizeof(void*) == 8) {
            handle = fake_dlopen("/system/lib64/libart.so", RTLD_NOW);
            jit_lib = fake_dlopen("/system/lib64/libart-compiler.so", RTLD_NOW);
        } else {
            handle = fake_dlopen("/system/lib/libart.so", RTLD_NOW);
            jit_lib = fake_dlopen("/system/lib/libart-compiler.so", RTLD_NOW);
        }
        jitCompileMethod = (bool (*)(void *, void *, void *, bool)) fake_dlsym(jit_lib, "jit_compile_method");
        jitLoad = reinterpret_cast<void* (*)(bool*)>(fake_dlsym(jit_lib, "jit_load"));
        bool generate_debug_info = false;
        jitCompilerHandle = (jitLoad)(&generate_debug_info);
    }
}

bool compileMethod(void* artMethod, void* thread) {
    return jitCompileMethod(jitCompilerHandle, artMethod, thread, false);
}


#endif //SANDHOOK_HIDE_API_H
