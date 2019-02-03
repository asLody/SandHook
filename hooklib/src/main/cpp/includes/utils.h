//
// Created by 甘尧 on 2019/1/13.
//

#ifndef SANDHOOK_UTILS_H
#define SANDHOOK_UTILS_H

#include <stdlib.h>
#include <sys/mman.h>
#include "jni.h"
#include "../includes/arch.h"
#include <unistd.h>
#include <sys/mman.h>

extern "C" {

    Size getAddressFromJava(JNIEnv *env, const char *className, const char *fieldName);

    Size getAddressFromJavaByCallMethod(JNIEnv *env, const char *className, const char *methodName);

    jint getIntFromJava(JNIEnv *env, const char *className, const char *fieldName);

    bool munprotect(size_t addr, size_t len);

    bool flushCacheExt(Size addr, Size len);

}


#endif //SANDHOOK_UTILS_H
