//
// Created by swift on 2019/1/21.
//

#ifndef SANDHOOK_HIDE_API_H
#define SANDHOOK_HIDE_API_H

#include <jni.h>
#include "../utils/fake_dlfcn.h"
#include "dlfcn.h"

extern "C" {

    void initHideApi(JNIEnv *env, int SDK_VERSION);
    bool compileMethod(void *artMethod, void *thread);

}

#endif //SANDHOOK_HIDE_API_H
