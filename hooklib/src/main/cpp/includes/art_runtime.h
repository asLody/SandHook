//
// Created by 甘尧 on 2019/2/23.
//

#ifndef SANDHOOK_ART_RUNTIME_H
#define SANDHOOK_ART_RUNTIME_H

#include "art_jit.h"
#include "art_instrumentation.h"

namespace art {
    class Runtime {
    public:
        instrumentation::Instrumentation* getInstrumentation();
    };
}

#endif //SANDHOOK_ART_RUNTIME_H
