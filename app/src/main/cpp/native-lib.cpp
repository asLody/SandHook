#include <jni.h>
#include <string>
#include "./casts/CastArtMethod.cpp"

extern "C" JNIEXPORT jstring

JNICALL
Java_com_swift_sandhook_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject instance,
        jobject method/* this */) {
    std::string hello = "Hello from C++";

    art::mirror::ArtMethod* artMethod = (art::mirror::ArtMethod*) env->FromReflectedMethod(method);
    artMethod->SetAccessFlags(0x0100);

    return env->NewStringUTF(hello.c_str());
}