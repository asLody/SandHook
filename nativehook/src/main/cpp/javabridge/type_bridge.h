//
// Created by 甘尧 on 2019-07-22.
//

#pragma once

#include "jni.h"

namespace SandHook {

    namespace JavaBridge {

        struct Types {

#define LANG_ClASS(c) static jclass java_lang_##c; static jmethodID java_lang_##c##_init; static jmethodID java_value_##c;

            LANG_ClASS(Integer);
            LANG_ClASS(Long);
            LANG_ClASS(Float);
            LANG_ClASS(Double);
            LANG_ClASS(Byte);
            LANG_ClASS(Short);
            LANG_ClASS(Boolean);
            LANG_ClASS(Character);

#undef LANG_ClASS

            static void Load(JNIEnv *env);


#define LANG_BOX_DEF(c, t) static jobject To##c(JNIEnv *env, t v);

#define LANG_UNBOX_V_DEF(k, c, t) static t From##c(JNIEnv *env, jobject j);

#define LANG_UNBOX_DEF(c, t) LANG_UNBOX_V_DEF(c, c, t)

            LANG_BOX_DEF(Object, jobject);

            LANG_BOX_DEF(Integer, jint);

            LANG_BOX_DEF(Long, jlong);

            LANG_BOX_DEF(Float, jfloat);

            LANG_BOX_DEF(Double, jdouble);

            LANG_BOX_DEF(Byte, jbyte);

            LANG_BOX_DEF(Short, jshort);

            LANG_BOX_DEF(Boolean, jboolean);

            LANG_BOX_DEF(Character, jchar);

            LANG_UNBOX_V_DEF(Int, Integer, jint);

            LANG_UNBOX_DEF(Object, jobject);

            LANG_UNBOX_DEF(Long, jlong);

            LANG_UNBOX_DEF(Float, jfloat);

            LANG_UNBOX_DEF(Double, jdouble);

            LANG_UNBOX_DEF(Byte, jbyte);

            LANG_UNBOX_DEF(Short, jshort);

            LANG_UNBOX_DEF(Boolean, jboolean);

            LANG_UNBOX_V_DEF(Char, Character, jchar);

#undef LANG_BOX_DEF
#undef LANG_UNBOX_V_DEF
#undef LANG_UNBOX_DEF
        };

#define EXPORT_LANG_ClASS(c) jclass Types::java_lang_##c;  jmethodID Types::java_lang_##c##_init; jmethodID Types::java_value_##c;

        EXPORT_LANG_ClASS(Integer);
        EXPORT_LANG_ClASS(Long);
        EXPORT_LANG_ClASS(Float);
        EXPORT_LANG_ClASS(Double);
        EXPORT_LANG_ClASS(Byte);
        EXPORT_LANG_ClASS(Short);
        EXPORT_LANG_ClASS(Boolean);
        EXPORT_LANG_ClASS(Character);

#undef EXPORT_LANG_ClASS


#define LANG_BOX(c, t) jobject Types::To##c(JNIEnv *env, t v) {  \
        return env->NewObject(Types::java_lang_##c, Types::java_lang_##c##_init, v);  \
}
#define LANG_UNBOX_V(k, c, t) t Types::From##c(JNIEnv *env, jobject j) {  \
        return env->Call##k##Method(j, Types::java_value_##c);  \
}
#define LANG_UNBOX(c, t) LANG_UNBOX_V(c, c, t)

        LANG_BOX(Integer, jint);
        LANG_BOX(Long, jlong);
        LANG_BOX(Float, jfloat);
        LANG_BOX(Double, jdouble);
        LANG_BOX(Byte, jbyte);
        LANG_BOX(Short, jshort);
        LANG_BOX(Boolean, jboolean);
        LANG_BOX(Character, jchar);

        jobject Types::ToObject(JNIEnv *env, jobject obj) {
            return obj;
        }

        LANG_UNBOX_V(Int, Integer, jint);
        LANG_UNBOX(Long, jlong);
        LANG_UNBOX(Float, jfloat);
        LANG_UNBOX(Double, jdouble);
        LANG_UNBOX(Byte, jbyte);
        LANG_UNBOX(Short, jshort);
        LANG_UNBOX(Boolean, jboolean);
        LANG_UNBOX_V(Char, Character, jchar);

        jobject Types::FromObject(JNIEnv *env, jobject obj) {
            return obj;
        }


#undef LANG_BOX
#undef LANG_UNBOX_V
#undef LANG_UNBOX


    }

}

