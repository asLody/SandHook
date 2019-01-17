#include <jni.h>
#include <string>
#include "casts/cast_art_method.h"
#include "./trampoline/trampoline.h"

extern "C" JNIEXPORT jstring

JNICALL
Java_com_swift_sandhook_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject instance,
        jobject method/* this */) {
    std::string hello = "Hello from C++";

//    art::mirror::ArtMethod* artMethod = (art::mirror::ArtMethod*) env->FromReflectedMethod(method);

    return env->NewStringUTF(hello.c_str());
}extern "C"
JNIEXPORT jint JNICALL
Java_com_swift_sandhook_MainActivity_calArtSize(JNIEnv *env, jobject instance, jobject method1,
                                                jobject method2) {
//    Size artMethod1 = (Size) env->FromReflectedMethod(method1);
//    Size artMethod2 = (Size) env->FromReflectedMethod(method2);
//
//    Size size = artMethod2 - artMethod1;
//
//    art::mirror::ArtMethod* methodTest = reinterpret_cast<art::mirror::ArtMethod*>(artMethod1);
//
//    Size addr1 = reinterpret_cast<size_t>(&methodTest->ptr_sized_fields_.entry_point_from_quick_compiled_code_);
//    Size addr2 = size + artMethod1 - BYTE_POINT;

    return 0;
}

extern void hook_trampoline();

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_MainActivity_initHook(JNIEnv *env, jobject instance) {

    void(*fp)() = hook_trampoline;

    SandHook::cast_art_method::init(env);

}