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

extern "C" {

Size getAddressFromJava(JNIEnv *env, char *className, char *fieldName);

Size getAddressFromJavaByCallMethod(JNIEnv *env, char *className, char *methodName);

jint getIntFromJava(JNIEnv *env, char *className, char *fieldName);

bool munprotect(size_t addr, size_t len);

}


#endif //SANDHOOK_UTILS_H
