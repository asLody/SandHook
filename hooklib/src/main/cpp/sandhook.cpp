#include <jni.h>
#include "includes/cast_art_method.h"
#include "includes/trampoline_manager.h"
#include "includes/hide_api.h"

SandHook::TrampolineManager trampolineManager;

int SDK_INT = 0;

enum HookMode {
    AUTO = 0,
    INLINE = 1,
    REPLACE = 2
};

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_initNative(JNIEnv *env, jclass type, jint sdk) {

    // TODO
    SDK_INT = sdk;
    SandHook::CastArtMethod::init(env, sdk);
    trampolineManager.init(sdk, SandHook::CastArtMethod::entryPointQuickCompiled->getOffset());
    initHideApi(env, sdk);
    return JNI_TRUE;

}

void disableCompilable(art::mirror::ArtMethod* method) {
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(method);
    if (SDK_INT >= ANDROID_O2) {
        accessFlag |= 0x02000000;
        accessFlag |= 0x00800000;
    } else {
        accessFlag |= 0x01000000;
    }
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

void tryDisableInline(art::mirror::ArtMethod* method) {
    if (method == nullptr)
        return;
    if (SDK_INT < ANDROID_O)
        return;
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(method);
    accessFlag &= ~ 0x08000000;
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

void disableInterpreterForO(art::mirror::ArtMethod* method) {
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(method);
    accessFlag |= 0x0100;
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

void setPrivate(art::mirror::ArtMethod* method) {
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(method);
    accessFlag &= ~ 0x1;
    accessFlag |= 0x2;
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

void setStatic(art::mirror::ArtMethod* method) {
    uint32_t accessFlag = SandHook::CastArtMethod::accessFlag->get(method);
    accessFlag |= 0x0008;
    SandHook::CastArtMethod::accessFlag->set(method, accessFlag);
}

bool isAbsMethod(art::mirror::ArtMethod* method) {
    uint32_t accessFlags = SandHook::CastArtMethod::accessFlag->get(method);
    return ((accessFlags & 0x0400) != 0);
}

bool isNative(art::mirror::ArtMethod* method) {
    uint32_t accessFlags = SandHook::CastArtMethod::accessFlag->get(method);
    return ((accessFlags & 0x0100) != 0);
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
        Size newCachedMethodsArray = reinterpret_cast<Size>(calloc(cacheSize, BYTE_POINT * 2));
        unsigned int one = 1;
        memcpy(reinterpret_cast<void *>(newCachedMethodsArray + BYTE_POINT), &one, 4);
        memcpy(reinterpret_cast<void *>(newCachedMethodsArray + BYTE_POINT * 2 * slotIndex),
               (&backupMethod),
               BYTE_POINT
        );
        memcpy(reinterpret_cast<void *>(newCachedMethodsArray + BYTE_POINT * 2 * slotIndex + BYTE_POINT),
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
    tryDisableInline(originMethod);
    if (backupMethod != nullptr) {
        memcpy(backupMethod, originMethod, SandHook::CastArtMethod::size);
        if (SDK_INT >= ANDROID_N) {
            disableCompilable(backupMethod);
        }
        if (SDK_INT >= ANDROID_O) {
            disableInterpreterForO(backupMethod);
        }
        tryDisableInline(backupMethod);
        setPrivate(backupMethod);
        SandHook::Trampoline::flushCache(reinterpret_cast<Size>(originMethod), SandHook::CastArtMethod::size);
    }
    if (SDK_INT >= ANDROID_O) {
        disableInterpreterForO(originMethod);
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

    if (SDK_INT >= ANDROID_N) {
        disableCompilable(originMethod);
        SandHook::Trampoline::flushCache(reinterpret_cast<Size>(originMethod), SandHook::CastArtMethod::size);
    }

    tryDisableInline(originMethod);

    SandHook::HookTrampoline* hookTrampoline = trampolineManager.installInlineTrampoline(originMethod, hookMethod, backupMethod);
    if (hookTrampoline == nullptr)
        return false;

    hookTrampoline->inlineSecondory->flushCache(reinterpret_cast<Size>(hookMethod), SandHook::CastArtMethod::size);
    if (hookTrampoline->callOrigin != nullptr) {
        //backup
        memcpy(backupMethod, originMethod, SandHook::CastArtMethod::size);

        SandHook::CastArtMethod::entryPointQuickCompiled->set(backupMethod, hookTrampoline->callOrigin->getCode());

        if (SDK_INT >= ANDROID_N) {
            disableCompilable(backupMethod);
        }
        if (SDK_INT >= ANDROID_O) {
            disableInterpreterForO(backupMethod);
        }
        tryDisableInline(backupMethod);
        setPrivate(backupMethod);
        hookTrampoline->callOrigin->flushCache(reinterpret_cast<Size>(backupMethod), SandHook::CastArtMethod::size);
    }
    return true;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_hookMethod(JNIEnv *env, jclass type, jobject originMethod,
                                            jobject hookMethod, jobject backupMethod, jint hookMode) {

    // TODO
    art::mirror::ArtMethod* origin = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(originMethod));
    art::mirror::ArtMethod* hook = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(hookMethod));
    art::mirror::ArtMethod* backup = backupMethod == NULL ? nullptr : reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(backupMethod));

    bool isInterpreter = SandHook::CastArtMethod::entryPointQuickCompiled->get(origin) == SandHook::CastArtMethod::quickToInterpreterBridge;

    int mode = reinterpret_cast<int>(hookMode);
    if (mode == INLINE) {
        if (isInterpreter) {
            if (SDK_INT >= ANDROID_N) {
                Size threadId = getAddressFromJavaByCallMethod(env, "com/swift/sandhook/SandHook", "getThreadId");
                if (compileMethod(origin, reinterpret_cast<void *>(threadId)) &&
                    SandHook::CastArtMethod::entryPointQuickCompiled->get(origin) !=
                    SandHook::CastArtMethod::quickToInterpreterBridge) {
                    return static_cast<jboolean>(doHookWithInline(env, origin, hook, backup));
                } else {
                    return static_cast<jboolean>(doHookWithReplacement(origin, hook, backup));
                }
            }
        } else {
            return static_cast<jboolean>(doHookWithInline(env, origin, hook, backup));
        }
    } else if (mode == REPLACE) {
        return static_cast<jboolean>(doHookWithReplacement(origin, hook, backup));
    }

    if (SDK_INT >= ANDROID_P && BYTE_POINT == 4) {
        bool isInterpreter = SandHook::CastArtMethod::entryPointQuickCompiled->get(hook) == SandHook::CastArtMethod::quickToInterpreterBridge;
        if (isInterpreter) {
            Size threadId = getAddressFromJavaByCallMethod(env, "com/swift/sandhook/SandHook", "getThreadId");
            compileMethod(hook, reinterpret_cast<void*>(threadId));
        }
        return static_cast<jboolean>(doHookWithReplacement(origin, hook, backup));
    } else if (isAbsMethod(origin)) {
        return static_cast<jboolean>(doHookWithReplacement(origin, hook, backup));
    } else if (isInterpreter) {
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

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_test_TestClass_jni_1test(JNIEnv *env, jobject instance) {
    int a = 1 + 1;
    int b = a + 1;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_ClassNeverCall_neverCallNative(JNIEnv *env, jobject instance) {

    // TODO
    int a = 1 + 1;
    int b = a + 1;

}