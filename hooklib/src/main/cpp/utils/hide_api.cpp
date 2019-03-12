//
// Created by swift on 2019/1/21.
//
#include "../includes/hide_api.h"
#include "../includes/arch.h"

extern int SDK_INT;

extern "C" {


    void* (*jitLoad)(bool*) = nullptr;
    void* jitCompilerHandle = nullptr;
    bool (*jitCompileMethod)(void*, void*, void*, bool) = nullptr;

    void (*innerSuspendVM)() = nullptr;
    void (*innerResumeVM)() = nullptr;

    jobject (*addWeakGlobalRef)(JavaVM *, void *, void *) = nullptr;

    art::jit::JitCompiler** globalJitCompileHandlerAddr = nullptr;



    void initHideApi(JNIEnv* env) {
        //init compile
        if (SDK_INT >= ANDROID_N) {
            void *jit_lib;
            if (BYTE_POINT == 8) {
                jit_lib = fake_dlopen("/system/lib64/libart-compiler.so", RTLD_NOW);
            } else {
                jit_lib = fake_dlopen("/system/lib/libart-compiler.so", RTLD_NOW);
            }
            jitCompileMethod = (bool (*)(void *, void *, void *, bool)) fake_dlsym(jit_lib, "jit_compile_method");
            jitLoad = reinterpret_cast<void* (*)(bool*)>(fake_dlsym(jit_lib, "jit_load"));
            bool generate_debug_info = false;
            jitCompilerHandle = (jitLoad)(&generate_debug_info);

            if (jitCompilerHandle != nullptr && jitCompilerHandle > 0) {
                art::CompilerOptions* compilerOptions = getCompilerOptions(
                        reinterpret_cast<art::jit::JitCompiler *>(jitCompilerHandle));
                disableJitInline(compilerOptions);
            }

        }
        //init suspend
        void* art_lib;
        const char* art_lib_path;
        if (BYTE_POINT == 8) {
            art_lib_path = "/system/lib64/libart.so";
        } else {
            art_lib_path = "/system/lib/libart.so";
        }
        if (SDK_INT >= ANDROID_N) {
            art_lib = fake_dlopen(art_lib_path, RTLD_NOW);
            if (art_lib > 0) {
                innerSuspendVM = reinterpret_cast<void (*)()>(fake_dlsym(art_lib,
                                                                         "_ZN3art3Dbg9SuspendVMEv"));
                innerResumeVM = reinterpret_cast<void (*)()>(fake_dlsym(art_lib,
                                                                        "_ZN3art3Dbg8ResumeVMEv"));
            }
        } else {
            art_lib = dlopen(art_lib_path, RTLD_NOW);
            if (art_lib > 0) {
                innerSuspendVM = reinterpret_cast<void (*)()>(dlsym(art_lib,
                                                                         "_ZN3art3Dbg9SuspendVMEv"));
                innerResumeVM = reinterpret_cast<void (*)()>(dlsym(art_lib,
                                                                        "_ZN3art3Dbg8ResumeVMEv"));
            }
        }

        //init for getObject & JitCompiler
        if (SDK_INT < ANDROID_M) {
            void *handle = dlopen("libart.so", RTLD_LAZY | RTLD_GLOBAL);
            addWeakGlobalRef = (jobject (*)(JavaVM *, void *, void *)) dlsym(handle,
                                                                                   "_ZN3art9JavaVMExt22AddWeakGlobalReferenceEPNS_6ThreadEPNS_6mirror6ObjectE");
        } else if (SDK_INT < ANDROID_N) {
            void *handle = dlopen("libart.so", RTLD_LAZY | RTLD_GLOBAL);
            addWeakGlobalRef = (jobject (*)(JavaVM *, void *, void *)) dlsym(handle,
                                                                                   "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadEPNS_6mirror6ObjectE");
        } else {
            void *handle;
            if (BYTE_POINT == 8) {
                handle = fake_dlopen("/system/lib64/libart.so", RTLD_NOW);
            } else {
                handle = fake_dlopen("/system/lib/libart.so", RTLD_NOW);
            }
            const char *addWeakGloablReferenceSymbol = SDK_INT <= 25
                                                       ? "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadEPNS_6mirror6ObjectE"
                                                       : "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadENS_6ObjPtrINS_6mirror6ObjectEEE";
            addWeakGlobalRef = (jobject (*)(JavaVM *, void *, void *)) fake_dlsym(handle,
                                                                                  addWeakGloablReferenceSymbol);

            //try disable inline !
            globalJitCompileHandlerAddr = reinterpret_cast<art::jit::JitCompiler **>(fake_dlsym(handle, "_ZN3art3jit3Jit20jit_compiler_handle_E"));
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

    bool canGetObject() {
        return addWeakGlobalRef != nullptr;
    }

    jobject getJavaObject(JNIEnv* env, void* thread, void* address) {

        if (addWeakGlobalRef == nullptr)
            return NULL;


        JavaVM *vm;
        env->GetJavaVM(&vm);

        jobject object = addWeakGlobalRef(vm, thread, address);
        if (object == NULL)
            return NULL;

        jobject result = env->NewLocalRef(object);
        env->DeleteWeakGlobalRef(object);

        return result;
    }

    art::jit::JitCompiler* getGlobalJitCompiler() {
        if (SDK_INT < ANDROID_N)
            return nullptr;
        if (globalJitCompileHandlerAddr == nullptr || globalJitCompileHandlerAddr <= 0)
            return nullptr;
        return *globalJitCompileHandlerAddr;
    }

    art::CompilerOptions* getCompilerOptions(art::jit::JitCompiler* compiler) {
        if (compiler == nullptr)
            return nullptr;
        return compiler->compilerOptions.get();
    }

    art::CompilerOptions* getGlobalCompilerOptions() {
        return getCompilerOptions(getGlobalJitCompiler());
    }

    bool disableJitInline(art::CompilerOptions* compilerOptions) {
        if (compilerOptions == nullptr || compilerOptions <= 0)
            return false;
        size_t originOptions = compilerOptions->getInlineMaxCodeUnits();
        //maybe a real inlineMaxCodeUnits
        if (originOptions > 0 && originOptions <= 1024) {
            compilerOptions->setInlineMaxCodeUnits(0);
            return true;
        } else {
            return false;
        }
    }

}

