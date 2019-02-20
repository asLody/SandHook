//
// Created by swift on 2019/1/21.
//

#ifndef SANDHOOK_HIDE_API_H
#define SANDHOOK_HIDE_API_H

#include <jni.h>
#include "dlfcn_nougat.h"
#include "dlfcn.h"

extern "C" {

    void initHideApi(JNIEnv *env);
    bool compileMethod(void *artMethod, void *thread);

    void suspendVM();
    void resumeVM();

    jobject getJavaObject(JNIEnv* env, void* thread, void* address);

}

#endif //SANDHOOK_HIDE_API_H
