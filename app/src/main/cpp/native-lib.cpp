#include <jni.h>
#include "casts/cast_art_method.h"
#include "./trampoline/trampoline_manager.h"
#include "./utils/hide_api.h"

SandHook::TrampolineManager trampolineManager;

int SDK_INT = 0;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_initNative(JNIEnv *env, jclass type, jint sdk) {

    // TODO
    SDK_INT = sdk;
    SandHook::CastArtMethod::init(env, sdk);
    trampolineManager.init(SandHook::CastArtMethod::entryPointQuickCompiled->getOffset());
    initHideApi(env, sdk);
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

bool doHookWithReplacement(art::mirror::ArtMethod *originMethod,
                           art::mirror::ArtMethod *hookMethod,
                           art::mirror::ArtMethod *backupMethod) {
    if (SDK_INT >= ANDROID_N) {
        disableCompilable(originMethod);
        disableCompilable(hookMethod);
    }
    if (SDK_INT >= ANDROID_O) {
        disableInterpreterForO(originMethod);
    }
    SandHook::HookTrampoline* hookTrampoline = trampolineManager.installReplacementTrampoline(originMethod, hookMethod, backupMethod);
    if (hookTrampoline != nullptr) {
        SandHook::CastArtMethod::entryPointQuickCompiled->set(originMethod, hookTrampoline->replacement->getCode());
        hookTrampoline->replacement->flushCache(reinterpret_cast<Size>(originMethod), 100);
    }
}

bool doHookWithInline(JNIEnv* env,
                      art::mirror::ArtMethod *originMethod,
                      art::mirror::ArtMethod *hookMethod,
                      art::mirror::ArtMethod *backupMethod) {

//    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(*originMethod);
//    SandHook::CastArtMethod::accessFlag->set(hookMethod, accessFlag);

    bool isInterpreter = SandHook::CastArtMethod::entryPointQuickCompiled->get(*hookMethod) == SandHook::CastArtMethod::quickToInterpreterBridge;

    if (isInterpreter) {
        Size threadId = getAddressFromJavaByCallMethod(env, "com/swift/sandhook/SandHook", "getThreadId");
        compileMethod(hookMethod, reinterpret_cast<void*>(threadId));
    }

    SandHook::HookTrampoline* hookTrampoline = trampolineManager.installInlineTrampoline(originMethod, hookMethod, backupMethod);
    hookTrampoline->inlineSecondory->flushCache(reinterpret_cast<Size>(hookMethod), 100);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_hookMethod(JNIEnv *env, jclass type, jobject originMethod,
                                            jobject hookMethod, jobject backupMethod) {

    // TODO
    art::mirror::ArtMethod* origin = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(originMethod));
    art::mirror::ArtMethod* hook = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(hookMethod));
    art::mirror::ArtMethod* backup = backupMethod == NULL ? nullptr : reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(backupMethod));

    bool isInterpreter = SandHook::CastArtMethod::entryPointQuickCompiled->get(*origin) == SandHook::CastArtMethod::quickToInterpreterBridge;

    if (isInterpreter) {
        if (SDK_INT >= ANDROID_N) {
            Size threadId = getAddressFromJavaByCallMethod(env, "com/swift/sandhook/SandHook", "getThreadId");
            if (compileMethod(origin, reinterpret_cast<void *>(threadId)) && SandHook::CastArtMethod::entryPointQuickCompiled->get(*origin) != SandHook::CastArtMethod::quickToInterpreterBridge) {
                doHookWithInline(env, origin, hook, backup);
            } else {
                doHookWithReplacement(origin, hook, backup);
            }
        } else {
            doHookWithReplacement(origin, hook, backup);
        }
    } else {
        doHookWithInline(env, origin, hook, backup);
    }

    return JNI_TRUE;

}

