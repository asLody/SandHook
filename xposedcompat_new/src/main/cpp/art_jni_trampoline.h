//
// Created by swift on 2019/6/3.
//

#ifndef SANDHOOK_ART_JNI_TRAMPOLINE_H
#define SANDHOOK_ART_JNI_TRAMPOLINE_H

#include <cstdint>
#include <jni.h>
#include <list>
#include <set>
#include "ffi_cxx.h"

jclass java_lang_Object;
JavaVM* javaVM;
jclass bridgeClass;
jmethodID bridgeMethod;


void* (*GetOatQuickMethodHeaderBackup)(void*,uintptr_t) = nullptr;


struct ArtHookParam {
    jint slot;
    bool is_static_;
    char returnShorty;
    char* paramsShorty;
};

std::list<ArtHookParam*> hookParams = std::list<ArtHookParam*>();
std::set<void*> hookMethods = std::set<void*>();

template<typename U, typename T>
U ForceCast(T *x) {
    return (U) (uintptr_t) x;
}

template<typename U, typename T>
U ForceCast(T &x) {
    return *(U *) &x;
}

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

static void UnBoxValue(JNIEnv *env, jvalue *jv, jobject obj, char type) {
    switch (type) {
        case 'I':
            jv->i = Types::FromInteger(env, obj);
            break;
        case 'Z':
            jv->z = Types::FromBoolean(env, obj);
            break;
        case 'J':
            jv->j = Types::FromLong(env, obj);
            break;
        case 'F':
            jv->f = Types::FromFloat(env, obj);
            break;
        case 'B':
            jv->b = Types::FromByte(env, obj);
            break;
        case 'D':
            jv->d = Types::FromDouble(env, obj);
            break;
        case 'S':
            jv->s = Types::FromShort(env, obj);
            break;
        case 'C':
            jv->c = Types::FromCharacter(env, obj);
            break;
        default:
            jv->l = obj;
    }
}

class QuickArgumentBuilder {
public:
    QuickArgumentBuilder(JNIEnv *env, size_t len) : env_(env), index_(0) {
        array_ = env->NewObjectArray(
                static_cast<jsize>(len),
                java_lang_Object,
                nullptr
        );
    }

#define APPEND_DEF(name, type)  \
    void Append##name(type value) {  \
        env_->SetObjectArrayElement(array_, index_++,  \
                                    Types::To##name(env_, value));  \
    }  \


    APPEND_DEF(Integer, jint)

    APPEND_DEF(Boolean, jboolean)

    APPEND_DEF(Byte, jbyte)

    APPEND_DEF(Character, jchar)

    APPEND_DEF(Short, jshort)

    APPEND_DEF(Float, jfloat)

    APPEND_DEF(Double, jdouble)

    APPEND_DEF(Long, jlong)

    APPEND_DEF(Object, jobject)


#undef APPEND_DEF

    jobjectArray GetArray() {
        return array_;
    }

private:
    JNIEnv *env_;
    jobjectArray array_;
    int index_;
};

#endif //SANDHOOK_ART_JNI_TRAMPOLINE_H
