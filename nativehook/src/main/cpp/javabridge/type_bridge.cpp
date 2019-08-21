//
// Created by 甘尧 on 2019-07-22.
//

#include "type_bridge.h"

using namespace SandHook::JavaBridge;

void Types::Load(JNIEnv *env) {
    jclass clazz;
    env->PushLocalFrame(16);

#define LOAD_CLASS(c, s)        clazz = env->FindClass(s); (c) = reinterpret_cast<jclass>(env->NewWeakGlobalRef(clazz))
#define LOAD_LANG_CLASS(c, s)   LOAD_CLASS(java_lang_##c, "java/lang/" #c); java_lang_##c##_init = env->GetMethodID(java_lang_##c, "<init>", s)

    LOAD_LANG_CLASS(Integer, "(I)V");
    LOAD_LANG_CLASS(Long, "(J)V");
    LOAD_LANG_CLASS(Float, "(F)V");
    LOAD_LANG_CLASS(Double, "(D)V");
    LOAD_LANG_CLASS(Byte, "(B)V");
    LOAD_LANG_CLASS(Short, "(S)V");
    LOAD_LANG_CLASS(Boolean, "(Z)V");
    LOAD_LANG_CLASS(Character, "(C)V");

#undef LOAD_CLASS
#undef LOAD_LANG_CLASS

#define LOAD_METHOD(k, c, r, s) java_value_##c = env->GetMethodID(k, r "Value", s)
#define LOAD_NUMBER(c, r, s)    LOAD_METHOD(java_lang_Number, c, r, s)

    jclass java_lang_Number = env->FindClass("java/lang/Number");

    LOAD_NUMBER(Integer, "int", "()I");
    LOAD_NUMBER(Long, "long", "()J");
    LOAD_NUMBER(Float, "float", "()F");
    LOAD_NUMBER(Double, "double", "()D");
    LOAD_NUMBER(Byte, "byte", "()B");
    LOAD_NUMBER(Short, "short", "()S");

    LOAD_METHOD(java_lang_Boolean, Boolean, "boolean", "()Z");
    LOAD_METHOD(java_lang_Character, Character, "char", "()C");

    env->PopLocalFrame(nullptr);
#undef LOAD_METHOD
#undef LOAD_NUMBER
}
