//
// Created by swift on 2019/6/3.
//

#include <cstring>
#include <utils/log.h>
#include "art_jni_trampoline.h"
#include "sandhook_native.h"

//bug fix hooks
void* NewGetOatQuickMethodHeader(void* artMethod, uintptr_t pc) {
    std::set<void*>::iterator it = hookMethods.find(artMethod);
    if (it != hookMethods.end()) {
        LOGW("skip GetOatQuickMethodHeader");
        return nullptr;
    }
    return GetOatQuickMethodHeaderBackup(artMethod, pc);
}
//bug fix hooks

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

void Types::Load(JNIEnv *env) {
    jclass clazz;
    env->PushLocalFrame(16);

#define LOAD_CLASS(c, s)        clazz = env->FindClass(s); c = reinterpret_cast<jclass>(env->NewWeakGlobalRef(clazz))
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

jobject InvokeHookedMethodBridge(JNIEnv *env, jint slot, jobject receiver,
                                     jobjectArray array) {
    return env->CallStaticObjectMethod(bridgeClass, bridgeMethod, slot, receiver, array);
}

template<typename RetType>
static RetType
InvokeJavaBridge(JNIEnv *env, ArtHookParam *param, jobject this_object,
                 jobjectArray arguments) {
    jobject ret = InvokeHookedMethodBridge(
            env,
            param->slot,
            this_object,
            arguments
    );
    jvalue val;
    UnBoxValue(env, &val, ret, param->returnShorty);
    return ForceCast<RetType>(val);
}

static void InvokeVoidJavaBridge(JNIEnv *env, ArtHookParam *param, jobject this_object,
                                 jobjectArray arguments) {
    InvokeHookedMethodBridge(
            env,
            param->slot,
            this_object,
            arguments
    );
}

FFIType FFIGetJniParameter(char shorty) {
    switch (shorty) {
        case 'Z':
            return FFIType::kFFITypeU1;
        case 'B':
            return FFIType::kFFITypeS1;
        case 'C':
            return FFIType::kFFITypeU2;
        case 'S':
            return FFIType::kFFITypeS2;
        case 'I':
            return FFIType::kFFITypeS4;
        case 'J':
            return FFIType::kFFITypeS8;
        case 'F':
            return FFIType::kFFITypeFloat;
        case 'D':
            return FFIType::kFFITypeDouble;
        case 'L':
            return FFIType::kFFITypePointer;
        case 'V':
            return FFIType::kFFITypeVoid;
        default:
            return FFIType::kFFITypePointer;
    }
}

void FFIJniDispatcher(FFIClosure *closure, void *resp, void **args, void *userdata) {
#define FFI_ARG(name, type)  \
    builder.Append##name(*reinterpret_cast<type *>(args[i]));

    ArtHookParam *param = reinterpret_cast<ArtHookParam *>(userdata);
    const char *argument = param->paramsShorty;
    unsigned int argument_len = (unsigned int) strlen(argument);
    JNIEnv *env = *reinterpret_cast<JNIEnv **>(args[0]);
    jobject this_object = nullptr;
    if (!param->is_static_) {
        this_object = *reinterpret_cast<jobject *>(args[1]);
    }
    // skip first two arguments
    args += 2;
    QuickArgumentBuilder builder(env, argument_len);

    for (int i = 0; i < argument_len; ++i) {
        switch (argument[i]) {
            case 'Z':
                FFI_ARG(Boolean, jboolean);
                break;
            case 'B':
                FFI_ARG(Byte, jbyte);
                break;
            case 'C':
                FFI_ARG(Character, jchar);
                break;
            case 'S':
                FFI_ARG(Short, jshort);
                break;
            case 'I':
                FFI_ARG(Integer, jint);
                break;
            case 'J':
                FFI_ARG(Long, jlong);
                break;
            case 'F':
                FFI_ARG(Float, jfloat);
                break;
            case 'D':
                FFI_ARG(Double, jdouble);
                break;
            case 'L':
                FFI_ARG(Object, jobject);
                break;
            default:
                FFI_ARG(Object, jobject);
                break;
        }
    }
#define INVOKE(type)  \
    *reinterpret_cast<type *>(resp) = InvokeJavaBridge<type>(env, param, this_object,  \
                                                                    builder.GetArray());

    switch (param->returnShorty) {
        case 'Z':
            INVOKE(jboolean);
            break;
        case 'B':
            INVOKE(jbyte);
            break;
        case 'C':
            INVOKE(jchar);
            break;
        case 'S':
            INVOKE(jshort);
            break;
        case 'I':
            INVOKE(jint);
            break;
        case 'J':
            INVOKE(jlong);
            break;
        case 'F':
            INVOKE(jfloat);
            break;
        case 'D':
            INVOKE(jdouble);
            break;
        case 'L':
            INVOKE(jobject);
            break;
        case 'V':
            InvokeVoidJavaBridge(env, param, this_object, builder.GetArray());
            break;
        default:
            InvokeVoidJavaBridge(env, param, this_object, builder.GetArray());
            break;
    }
#undef INVOKE
#undef FFI_ARG
}


FFIClosure* BuildJniClosure(ArtHookParam *param) {
    const char *argument = param->paramsShorty;
    unsigned int java_argument_len = (unsigned int) strlen(argument);
    unsigned int jni_argument_len = java_argument_len + 2;
    FFICallInterface *cif = new FFICallInterface(
            FFIGetJniParameter(param->returnShorty)
    );
    cif->Parameter(FFIType::kFFITypePointer);  // JNIEnv *
    cif->Parameter(FFIType::kFFITypePointer);  // jclass or jobject
    for (int i = 2; i < jni_argument_len; ++i) {
        cif->Parameter(FFIGetJniParameter(argument[i - 2]));
    }
    cif->FinalizeCif();
    return cif->CreateClosure(param, FFIJniDispatcher);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_xposedcompat_XposedCompat_init(JNIEnv *env, jclass type, jclass jbridgeClass, jobject jbridgeMethod, jclass jobjClass) {
    java_lang_Object = static_cast<jclass>(env->NewGlobalRef(jobjClass));
    bridgeClass = static_cast<jclass>(env->NewGlobalRef(jbridgeClass));
    bridgeMethod = env->FromReflectedMethod(jbridgeMethod);
    env->GetJavaVM(&javaVM);
    Types::Load(env);
    if (sizeof(size_t) == 8) {
        GetOatQuickMethodHeaderBackup = reinterpret_cast<void *(*)(void *,
                                                              uintptr_t)>(SandInlineHookSym("/system/lib64/libart.so", "_ZN3art9ArtMethod23GetOatQuickMethodHeaderEm", (void*)NewGetOatQuickMethodHeader));
    } else {
        GetOatQuickMethodHeaderBackup = reinterpret_cast<void *(*)(void *,
                                                                   uintptr_t)>(SandInlineHookSym("/system/lib/libart.so", "_ZN3art9ArtMethod23GetOatQuickMethodHeaderEj", (void*)NewGetOatQuickMethodHeader));
    }
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_swift_sandhook_xposedcompat_XposedCompat_getJNITrampoline(JNIEnv *env, jclass type, jint slot, jboolean isStatic, jchar retShorty, jcharArray paramShorty) {
    ArtHookParam* artHookParam = new ArtHookParam();
    hookParams.push_back(artHookParam);
    artHookParam->is_static_ = isStatic;
    artHookParam->slot = slot;
    artHookParam->returnShorty = static_cast<char>(retShorty);
    jchar* jcs = env->GetCharArrayElements(paramShorty, NULL);
    char* params = new char[env->GetArrayLength(paramShorty) + 1];
    for (int i = 0; i < env->GetArrayLength(paramShorty); ++i) {
        params[i] = static_cast<char>(jcs[i]);
    }
    params[env->GetArrayLength(paramShorty)] = '\0';
    artHookParam->paramsShorty = params;
    env->ReleaseCharArrayElements(paramShorty, jcs, 0);
    FFIClosure* closure = BuildJniClosure(artHookParam);
    if (closure != nullptr) {
        return reinterpret_cast<jlong>(closure->GetCode());
    } else {
        return 0;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_xposedcompat_XposedCompat_addHookMethod(JNIEnv *env, jclass type, jobject hookMethod) {
    hookMethods.insert(env->FromReflectedMethod(hookMethod));
}


static JNINativeMethod jniXpCompat[] = {
        {
                "init",
                "(Ljava/lang/Class;Ljava/lang/reflect/Method;Ljava/lang/Class;)V",
                (void *) Java_com_swift_sandhook_xposedcompat_XposedCompat_init
        },
        {
                "getJNITrampoline",
                "(IZC[C)J",
                (void *) Java_com_swift_sandhook_xposedcompat_XposedCompat_getJNITrampoline
        },
        {
                "addHookMethod",
                "(Ljava/lang/reflect/Member;)V",
                (void *) Java_com_swift_sandhook_xposedcompat_XposedCompat_addHookMethod
        }
};


static bool registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *jniMethods, int methods) {
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        return false;
    }
    return env->RegisterNatives(clazz, jniMethods, methods) >= 0;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    const char* CLASS_XP_COMPAT = "com/swift/sandhook/xposedcompat/XposedCompat";

    int jniMethodSize = sizeof(JNINativeMethod);

    JNIEnv *env = NULL;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    if (!registerNativeMethods(env, CLASS_XP_COMPAT, jniXpCompat, sizeof(jniXpCompat) / jniMethodSize)) {
        return -1;
    }

    LOGW("JNI Loaded");

    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT bool JNI_Load_Ex(JNIEnv* env, jclass classXpComapt) {
    int jniMethodSize = sizeof(JNINativeMethod);

    if (env == nullptr || classXpComapt == nullptr)
        return false;

    if (env->RegisterNatives(classXpComapt, jniXpCompat, sizeof(jniXpCompat) / jniMethodSize) < 0) {
        return false;
    }

    LOGW("JNI Loaded");
    return true;
}

