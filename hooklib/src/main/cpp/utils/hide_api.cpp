//
// Created by swift on 2019/1/21.
//
#include "../includes/hide_api.h"
#include "../includes/arch.h"
#include "../includes/elf_util.h"
#include "../includes/log.h"
#include "../includes/utils.h"

extern int SDK_INT;

extern "C" {


    void* (*jitLoad)(bool*) = nullptr;
    void* jitCompilerHandle = nullptr;
    bool (*jitCompileMethod)(void*, void*, void*, bool) = nullptr;
    bool (*jitCompileMethodQ)(void*, void*, void*, bool, bool) = nullptr;

    void (*innerSuspendVM)() = nullptr;
    void (*innerResumeVM)() = nullptr;

    jobject (*addWeakGlobalRef)(JavaVM *, void *, void *) = nullptr;

    art::jit::JitCompiler** globalJitCompileHandlerAddr = nullptr;

    //for Android Q
    void (**origin_jit_update_options)(void *) = nullptr;

    void (*profileSaver_ForceProcessProfiles)() = nullptr;

    const char* art_lib_path;
    const char* jit_lib_path;

    JavaVM* jvm;

    void initHideApi(JNIEnv* env) {

        env->GetJavaVM(&jvm);

        if (BYTE_POINT == 8) {
            art_lib_path = "/system/lib64/libart.so";
            jit_lib_path = "/system/lib64/libart-compiler.so";
        } else {
            art_lib_path = "/system/lib/libart.so";
            jit_lib_path = "/system/lib/libart-compiler.so";
        }

        //init compile
        if (SDK_INT >= ANDROID_N) {
            if (SDK_INT >= ANDROID_Q) {
                jitCompileMethodQ = reinterpret_cast<bool (*)(void *, void *, void *, bool,
                                                         bool)>(getSymCompat(jit_lib_path, "jit_compile_method"));
            } else {
                jitCompileMethod = reinterpret_cast<bool (*)(void *, void *, void *,
                                                             bool)>(getSymCompat(jit_lib_path,
                                                                                 "jit_compile_method"));
            }
            jitLoad = reinterpret_cast<void* (*)(bool*)>(getSymCompat(jit_lib_path, "jit_load"));
            bool generate_debug_info = false;
            jitCompilerHandle = (jitLoad)(&generate_debug_info);

            if (jitCompilerHandle != nullptr && jitCompilerHandle > 0) {
                art::CompilerOptions* compilerOptions = getCompilerOptions(
                        reinterpret_cast<art::jit::JitCompiler *>(jitCompilerHandle));
                disableJitInline(compilerOptions);
            }

        }


        //init suspend
        innerSuspendVM = reinterpret_cast<void (*)()>(getSymCompat(art_lib_path,
                                                                         "_ZN3art3Dbg9SuspendVMEv"));
        innerResumeVM = reinterpret_cast<void (*)()>(getSymCompat(art_lib_path,
                                                                        "_ZN3art3Dbg8ResumeVMEv"));


        //init for getObject & JitCompiler
        const char* add_weak_ref_sym;
        if (SDK_INT < ANDROID_M) {
            add_weak_ref_sym = "_ZN3art9JavaVMExt22AddWeakGlobalReferenceEPNS_6ThreadEPNS_6mirror6ObjectE";
        } else if (SDK_INT < ANDROID_N) {
            add_weak_ref_sym = "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadEPNS_6mirror6ObjectE";
        } else  {
            add_weak_ref_sym = SDK_INT <= ANDROID_N2
                                           ? "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadEPNS_6mirror6ObjectE"
                                           : "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadENS_6ObjPtrINS_6mirror6ObjectEEE";
        }

        addWeakGlobalRef = reinterpret_cast<jobject (*)(JavaVM *, void *,
                                                   void *)>(getSymCompat(art_lib_path, add_weak_ref_sym));

        if (SDK_INT >= ANDROID_N) {
            globalJitCompileHandlerAddr = reinterpret_cast<art::jit::JitCompiler **>(getSymCompat(art_lib_path, "_ZN3art3jit3Jit20jit_compiler_handle_E"));
        }

        if (SDK_INT >= ANDROID_Q) {
            origin_jit_update_options = reinterpret_cast<void (**)(void *)>(getSymCompat(art_lib_path, "_ZN3art3jit3Jit20jit_update_options_E"));
        }

        if (SDK_INT > ANDROID_N) {
            profileSaver_ForceProcessProfiles = reinterpret_cast<void (*)()>(getSymCompat(art_lib_path, "_ZN3art12ProfileSaver20ForceProcessProfilesEv"));
        }

    }

    bool canCompile() {
        if (getGlobalJitCompiler() == nullptr || getGlobalJitCompiler() <= 0) {
            LOGE("JIT not init!");
            return false;
        }
        JNIEnv *env;
        jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        return getBooleanFromJava(env, "com/swift/sandhook/SandHookConfig",
                                  "compiler");
    }

    bool compileMethod(void* artMethod, void* thread) {
        if (!canCompile()) return false;
        if (SDK_INT >= ANDROID_Q) {
            if (jitCompileMethodQ == nullptr) {
                return false;
            }
            return jitCompileMethodQ(jitCompilerHandle, artMethod, thread, false, false);
        } else {
            if (jitCompileMethod == nullptr) {
                return false;
            }
            return jitCompileMethod(jitCompilerHandle, artMethod, thread, false);
        }
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

        jobject object = addWeakGlobalRef(jvm, thread, address);
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

    void* getInterpreterBridge(bool isNative) {
        SandHook::ElfImg libart(art_lib_path);
        if (isNative) {
            return reinterpret_cast<void *>(libart.getSymbAddress("art_quick_generic_jni_trampoline"));
        } else {
            return reinterpret_cast<void *>(libart.getSymbAddress("art_quick_to_interpreter_bridge"));
        }
    }

    //to replace jit_update_option
    void fake_jit_update_options(void* handle) {
        //do nothing
        LOGW("android q: art request update compiler options");
        return;
    }

    bool replaceUpdateCompilerOptionsQ() {
        if (SDK_INT < ANDROID_Q)
            return false;
        if (origin_jit_update_options == nullptr
            || origin_jit_update_options <= 0
            || *origin_jit_update_options == nullptr
            || *origin_jit_update_options <= 0)
            return false;
        *origin_jit_update_options = fake_jit_update_options;
    }

    bool forceProcessProfiles() {
        if (profileSaver_ForceProcessProfiles == nullptr)
            return false;
        profileSaver_ForceProcessProfiles();
    }

}

