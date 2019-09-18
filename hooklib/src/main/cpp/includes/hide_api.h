//
// Created by swift on 2019/1/21.
//

#ifndef SANDHOOK_HIDE_API_H
#define SANDHOOK_HIDE_API_H

#include <jni.h>
#include "dlfcn_nougat.h"
#include "dlfcn.h"
#include <memory>
#include "../includes/art_compiler_options.h"
#include "../includes/art_jit.h"

extern "C" {

    void initHideApi(JNIEnv *env);
    bool compileMethod(void *artMethod, void *thread);

    void suspendVM();
    void resumeVM();

    bool canGetObject();
    jobject getJavaObject(JNIEnv* env, void* thread, void* address);

    art::jit::JitCompiler* getGlobalJitCompiler();

    art::CompilerOptions* getCompilerOptions(art::jit::JitCompiler* compiler);

    art::CompilerOptions* getGlobalCompilerOptions();

    bool disableJitInline(art::CompilerOptions* compilerOptions);

    void* getInterpreterBridge(bool isNative);

    bool replaceUpdateCompilerOptionsQ();

    bool forceProcessProfiles();

    bool hookClassInit(void(*callback)(void*));

    JNIEnv *attachAndGetEvn();

}

#endif //SANDHOOK_HIDE_API_H
