//
// Created by SwiftGan on 2019/2/28.
//

#ifndef SANDHOOK_CAST_RUNTIME_H
#define SANDHOOK_CAST_RUNTIME_H

#include "art_runtime.h"
#include "art_instrumentation.h"
#include "cast.h"

namespace SandHook {

    class CastRuntime {
        static void init(JNIEnv *jniEnv);
        static IMember<art::Runtime, art::instrumentation::Instrumentation*>* instrumentation;
    };

}

#endif //SANDHOOK_CAST_RUNTIME_H
