//
// Created by swift on 2019/2/3.
//

#include "../includes/utils.h"


Size getAddressFromJava(JNIEnv *env, char *className, char *fieldName) {
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        printf("find class error !");
        return 0;
    }
    jfieldID id = env->GetStaticFieldID(clazz, fieldName, "J");
    if (id == NULL) {
        printf("find field error !");
        return 0;
    }
    return env->GetStaticLongField(clazz, id);
}

Size getAddressFromJavaByCallMethod(JNIEnv *env, char *className, char *methodName) {
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        printf("find class error !");
        return 0;
    }
    jmethodID id = env->GetStaticMethodID(clazz, methodName, "()J");
    if (id == NULL) {
        printf("find field error !");
        return 0;
    }
    return env->CallStaticLongMethodA(clazz, id, nullptr);
}

jint getIntFromJava(JNIEnv *env, char *className, char *fieldName) {
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        printf("find class error !");
        return 0;
    }
    jfieldID id = env->GetStaticFieldID(clazz, fieldName, "I");
    if (id == NULL) {
        printf("find field error !");
        return 0;
    }
    return env->GetStaticIntField(clazz, id);
}

bool munprotect(size_t addr, size_t len) {
    long pagesize = sysconf(_SC_PAGESIZE);
    unsigned alignment = (unsigned) ((unsigned long long) addr % pagesize);
    int i = mprotect((void *) (addr - alignment), (size_t) (alignment + len),
                     PROT_READ | PROT_WRITE | PROT_EXEC);
    if (i == -1) {
        return false;
    }
    return true;
}

