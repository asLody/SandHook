//
// Created by swift on 2019/1/21.
//
#include "../includes/hide_api.h"

extern "C" {


    void* (*jitLoad)(bool*) = nullptr;
    void* jitCompilerHandle = nullptr;
    bool (*jitCompileMethod)(void*, void*, void*, bool) = nullptr;

    void initHideApi(JNIEnv* env, int SDK_VERSION) {
        if (SDK_VERSION >= 24) {
            void *jit_lib;
            if (sizeof(void*) == 8) {
                jit_lib = fake_dlopen("/system/lib64/libart-compiler.so", RTLD_NOW);
            } else {
                jit_lib = fake_dlopen("/system/lib/libart-compiler.so", RTLD_NOW);
            }
            jitCompileMethod = (bool (*)(void *, void *, void *, bool)) fake_dlsym(jit_lib, "jit_compile_method");
            jitLoad = reinterpret_cast<void* (*)(bool*)>(fake_dlsym(jit_lib, "jit_load"));
            bool generate_debug_info = false;
            jitCompilerHandle = (jitLoad)(&generate_debug_info);
        }
    }

    bool compileMethod(void* artMethod, void* thread) {
        if (jitCompileMethod == nullptr) {
            return false;
        }
        return jitCompileMethod(jitCompilerHandle, artMethod, thread, false);
    }

}

