#include <jni.h>
#include "casts/cast_art_method.h"
#include "./trampoline/trampoline_manager.h"

SandHook::TrampolineManager trampolineManager;

int SDK_INT = 0;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_initNative(JNIEnv *env, jclass type, jint sdk) {

    // TODO
    SDK_INT = sdk;
    SandHook::CastArtMethod::init(env, sdk);
    trampolineManager.init(SandHook::CastArtMethod::entryPointQuickCompiled->getOffset());
    return JNI_TRUE;

}

void disableCompilable(art::mirror::ArtMethod* method) {
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(*method);
    accessFlag |= 0x01000000;
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

void disableInterpreterForO(art::mirror::ArtMethod* method) {
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(*method);
    accessFlag |= 0x0100;
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_hookMethod(JNIEnv *env, jclass type, jobject originMethod,
                                            jobject hookMethod, jobject backupMethod) {

    // TODO
    art::mirror::ArtMethod* origin = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(originMethod));
    art::mirror::ArtMethod* hook = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(hookMethod));
    art::mirror::ArtMethod* backup = backupMethod == NULL ? nullptr : reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(backupMethod));

//    if (SandHook::CastArtMethod::entryPointQuickCompiled->get(*origin) == SandHook::CastArtMethod::quickToInterpreterBridge) {
//        return JNI_FALSE;
//    }

    if (SDK_INT >= ANDROID_N) {
        disableCompilable(origin);
        disableCompilable(hook);
    }

    if (SDK_INT >= ANDROID_O) {
        disableInterpreterForO(origin);
    }

    SandHook::HookTrampoline* hookTrampoline = trampolineManager.installReplacementTrampoline(origin, hook, backup);
    if (hookTrampoline != nullptr) {
        SandHook::CastArtMethod::entryPointQuickCompiled->set(origin, hookTrampoline->replacement->getCode());
        hookTrampoline->replacement->flushCache(reinterpret_cast<uint64_t>(origin), 100);
    }

    return JNI_TRUE;

}