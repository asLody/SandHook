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
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(method);
    accessFlag |= 0x01000000;
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

void disableInterpreterForO(art::mirror::ArtMethod* method) {
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(method);
    accessFlag |= 0x0100;
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

void ensureMethodCached(art::mirror::ArtMethod *hookMethod, art::mirror::ArtMethod *backupMethod) {
    if (SDK_INT >= ANDROID_P)
        return;
    uint32_t index = SandHook::CastArtMethod::dexMethodIndex->get(backupMethod);
    if (SDK_INT < ANDROID_O2) {
        SandHook::CastArtMethod::dexCacheResolvedMethods->setElement(hookMethod, index, backupMethod);
    } else {
        int cacheSize = 1024;
        Size slotIndex = index % cacheSize;
        void *newCachedMethodsArray = calloc(cacheSize, BYTE_POINT * 2);
        unsigned int one = 1;
        memcpy(newCachedMethodsArray + BYTE_POINT, &one, 4);
        memcpy(newCachedMethodsArray + BYTE_POINT * 2 * slotIndex,
               (&backupMethod),
               BYTE_POINT
        );
        memcpy(newCachedMethodsArray + BYTE_POINT * 2 * slotIndex + BYTE_POINT,
               &index,
               4
        );
        SandHook::CastArtMethod::dexCacheResolvedMethods->set(hookMethod, &newCachedMethodsArray);
    }
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
    if (backupMethod != nullptr) {
        memcpy(backupMethod, originMethod, SandHook::CastArtMethod::size);
    }
    SandHook::HookTrampoline* hookTrampoline = trampolineManager.installReplacementTrampoline(originMethod, hookMethod, backupMethod);
    if (hookTrampoline != nullptr) {
        SandHook::CastArtMethod::entryPointQuickCompiled->set(originMethod, hookTrampoline->replacement->getCode());
        void* entryPointFormInterpreter = SandHook::CastArtMethod::entryPointFormInterpreter->get(hookMethod);
        if (entryPointFormInterpreter != NULL) {
            SandHook::CastArtMethod::entryPointFormInterpreter->set(originMethod, entryPointFormInterpreter);
        }
        hookTrampoline->replacement->flushCache(reinterpret_cast<Size>(originMethod), SandHook::CastArtMethod::size);
        return true;
    } else {
        return false;
    }
}

bool doHookWithInline(JNIEnv* env,
                      art::mirror::ArtMethod *originMethod,
                      art::mirror::ArtMethod *hookMethod,
                      art::mirror::ArtMethod *backupMethod) {

    bool isInterpreter = SandHook::CastArtMethod::entryPointQuickCompiled->get(hookMethod) == SandHook::CastArtMethod::quickToInterpreterBridge;

    //fix >= 8.1
    if (isInterpreter) {
        Size threadId = getAddressFromJavaByCallMethod(env, "com/swift/sandhook/SandHook", "getThreadId");
        compileMethod(hookMethod, reinterpret_cast<void*>(threadId));
    }

    SandHook::HookTrampoline* hookTrampoline = trampolineManager.installInlineTrampoline(originMethod, hookMethod, backupMethod);
    if (hookTrampoline == nullptr)
        return false;
    void* entryPointFormInterpreter = SandHook::CastArtMethod::entryPointFormInterpreter->get(hookMethod);
    if (entryPointFormInterpreter != NULL) {
        SandHook::CastArtMethod::entryPointFormInterpreter->set(originMethod, entryPointFormInterpreter);
    }
    hookTrampoline->inlineSecondory->flushCache(reinterpret_cast<Size>(hookMethod), SandHook::CastArtMethod::size);
    if (hookTrampoline->callOrigin != nullptr) {
        //backup
        memcpy(backupMethod, originMethod, SandHook::CastArtMethod::size);

        Code callOriginCode = hookTrampoline->callOrigin->getCode();
        if (hookTrampoline->callOrigin->isThumCode()) {
            callOriginCode = hookTrampoline->callOrigin->getThumbCodePcAddress(callOriginCode);
        }
        SandHook::CastArtMethod::entryPointQuickCompiled->set(backupMethod, callOriginCode);

        if (SDK_INT >= ANDROID_N) {
            disableCompilable(backupMethod);
        }
        if (SDK_INT >= ANDROID_O) {
            disableInterpreterForO(backupMethod);
        }

        hookTrampoline->callOrigin->flushCache(reinterpret_cast<Size>(backupMethod), SandHook::CastArtMethod::size);
    }
    return true;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_hookMethod(JNIEnv *env, jclass type, jobject originMethod,
                                            jobject hookMethod, jobject backupMethod) {

    // TODO
    art::mirror::ArtMethod* origin = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(originMethod));
    art::mirror::ArtMethod* hook = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(hookMethod));
    art::mirror::ArtMethod* backup = backupMethod == NULL ? nullptr : reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(backupMethod));

//    if (backup != nullptr) {
//        memcpy(backup, origin, SandHook::CastArtMethod::size);
//    }

    bool isInterpreter = SandHook::CastArtMethod::entryPointQuickCompiled->get(origin) == SandHook::CastArtMethod::quickToInterpreterBridge;

//    #if defined(__arm__)
//        doHookWithReplacement(origin, hook, backup);
//        return JNI_TRUE;
//    #endif

    if (isInterpreter) {
        if (SDK_INT >= ANDROID_N) {
            Size threadId = getAddressFromJavaByCallMethod(env, "com/swift/sandhook/SandHook", "getThreadId");
            if (compileMethod(origin, reinterpret_cast<void *>(threadId)) && SandHook::CastArtMethod::entryPointQuickCompiled->get(origin) != SandHook::CastArtMethod::quickToInterpreterBridge) {
                return static_cast<jboolean>(doHookWithInline(env, origin, hook, backup));
            } else {
                return static_cast<jboolean>(doHookWithReplacement(origin, hook, backup));
            }
        } else {
            return static_cast<jboolean>(doHookWithReplacement(origin, hook, backup));
        }
    } else {
        return static_cast<jboolean>(doHookWithInline(env, origin, hook, backup));
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_SandHook_ensureMethodCached(JNIEnv *env, jclass type, jobject hook,
                                                    jobject backup) {
    art::mirror::ArtMethod* hookeMethod = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(hook));
    art::mirror::ArtMethod* backupMethod = backup == NULL ? nullptr : reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(backup));
    ensureMethodCached(hookeMethod, backupMethod);
}