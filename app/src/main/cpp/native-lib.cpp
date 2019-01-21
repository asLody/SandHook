#include <jni.h>
#include "casts/cast_art_method.h"
#include "./trampoline/trampoline_manager.h"

SandHook::TrampolineManager trampolineManager;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_initNative(JNIEnv *env, jclass type, jint sdk) {

    // TODO
    SandHook::CastArtMethod::init(env, sdk);
    trampolineManager.init(SandHook::CastArtMethod::entryPointQuickCompiled->getOffset());
    return JNI_TRUE;

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_hookMethod(JNIEnv *env, jclass type, jobject originMethod,
                                            jobject hookMethod, jobject backupMethod) {

    // TODO
    art::mirror::ArtMethod* origin = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(originMethod));
    art::mirror::ArtMethod* hook = reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(hookMethod));
    art::mirror::ArtMethod* backup = backupMethod == NULL ? nullptr : reinterpret_cast<art::mirror::ArtMethod *>(env->FromReflectedMethod(backupMethod));

    trampolineManager.installInlineTrampoline(origin, hook, backup);

    return JNI_TRUE;

}