//
// Created by SwiftGan on 2019/4/12.
//

#ifndef SANDHOOK_SANDHOOK_H
#define SANDHOOK_SANDHOOK_H

#include <jni.h>

extern "C"
JNIEXPORT bool nativeHookNoBackup(void* origin, void* hook);

#endif //SANDHOOK_SANDHOOK_H
