#include <jni.h>
#include "casts/cast_art_method.h"
#include "./trampoline/trampoline_manager.h"

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_swift_sandhook_SandHook_initNative(JNIEnv *env, jclass type, jint sdk) {

    // TODO
    SandHook::CastArtMethod::init(env, sdk);

    return JNI_TRUE;

}