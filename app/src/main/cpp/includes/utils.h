//
// Created by 甘尧 on 2019/1/13.
//

#ifndef SANDHOOK_UTILS_H
#define SANDHOOK_UTILS_H

#include <stdlib.h>
#include <sys/mman.h>
#include "jni.h"
#include <unistd.h>

template<typename T>
int findOffset(void *start, size_t len,size_t step,T value) {

    if (NULL == start) {
        return -1;
    }

    for (int i = 0; i <= len; i += step) {
        T current_value = *reinterpret_cast<T*>((size_t)start + i);
        if (value == current_value) {
            return i;
        }
    }
    return -1;
}

template<typename T>
int findOffsetWithCB1(void *start, size_t len,size_t step, bool func(int, T)) {

    if (NULL == start) {
        return -1;
    }

    for (int i = 0; i <= len; i += step) {
        T current_value = *reinterpret_cast<T*>((size_t)start + i);
        if (func(i, current_value)) {
            return i;
        }
    }
    return -1;
}

template<typename T>
int findOffsetWithCB2(void *start1, void *start2, size_t len,size_t step, bool func(T,T)) {

    if (NULL == start1 || NULL == start2) {
        return -1;
    }

    for (int i = 0; i <= len; i += step) {
        T v1 = *reinterpret_cast<T*>((size_t)start1 + i);
        T v2 = *reinterpret_cast<T*>((size_t)start2 + i);
        if (func(v1, v2)) {
            return i;
        }
    }

    return -1;
}


Size getAddressFromJava(JNIEnv* env, char* className, char* fieldName) {
    jclass clazz = env -> FindClass(className);
    if (clazz == NULL){
        printf("find class error !");
        return 0;
    }
    jfieldID id = env -> GetStaticFieldID(clazz, fieldName, "J");
    if (id == NULL){
        printf("find field error !");
        return 0;
    }
    return env -> GetStaticLongField(clazz, id);
}

jint getIntFromJava(JNIEnv* env, char* className, char* fieldName) {
    jclass clazz = env -> FindClass(className);
    if (clazz == NULL){
        printf("find class error !");
        return 0;
    }
    jfieldID id = env -> GetStaticFieldID(clazz, fieldName, "I");
    if (id == NULL){
        printf("find field error !");
        return 0;
    }
    return env -> GetStaticIntField(clazz, id);
}

bool munprotect(size_t addr, size_t len) {
    long pagesize = sysconf(_SC_PAGESIZE);
    unsigned alignment = (unsigned)((unsigned long long)addr % pagesize);
    int i = mprotect((void *) (addr - alignment), (size_t) (alignment + len),
                     PROT_READ | PROT_WRITE | PROT_EXEC);
    if (i == -1) {
        return false;
    }
    return true;
}



#endif //SANDHOOK_UTILS_H
