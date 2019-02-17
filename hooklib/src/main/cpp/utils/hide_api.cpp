//
// Created by swift on 2019/1/21.
//
#include "../includes/hide_api.h"

extern int SDK_INT;

extern "C" {


    void* (*jitLoad)(bool*) = nullptr;
    void* jitCompilerHandle = nullptr;
    bool (*jitCompileMethod)(void*, void*, void*, bool) = nullptr;

    void (*innerSuspendVM)() = nullptr;
    void (*innerResumeVM)() = nullptr;


    void initHideApi(JNIEnv* env) {
        //init compile
        if (SDK_INT >= 24) {
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
        //init suspend
        void* art_lib;
        const char* art_lib_path;
        if (sizeof(void*) == 8) {
            art_lib_path = "/system/lib64/libart.so";
        } else {
            art_lib_path = "/system/lib/libart.so";
        }
        if (SDK_INT >= 24) {
            art_lib = fake_dlopen(art_lib_path, RTLD_NOW);
            if (art_lib > 0) {
                innerSuspendVM = reinterpret_cast<void (*)()>(fake_dlsym(art_lib,
                                                                         "_ZN3art3Dbg9SuspendVMEv"));
                innerResumeVM = reinterpret_cast<void (*)()>(fake_dlsym(art_lib,
                                                                        "_ZN3art3Dbg8ResumeVMEv"));
            }
        }
    }

    bool compileMethod(void* artMethod, void* thread) {
        if (jitCompileMethod == nullptr) {
            return false;
        }
        return jitCompileMethod(jitCompilerHandle, artMethod, thread, false);
    }

    void suspendVM() {
        if (innerSuspendVM == nullptr || innerResumeVM == nullptr)
            return;
        innerSuspendVM();
    }

    void resumeVM() {
        if (innerSuspendVM == nullptr || innerResumeVM == nullptr)
            return;
        innerResumeVM();
    }

}

