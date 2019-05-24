//
// Created by swift on 2019/5/14.
//

#ifndef SANDHOOK_NH_HOOK_H
#define SANDHOOK_NH_HOOK_H

#include <mutex>

#include "code_buffer.h"
#include "decoder.h"
#include "assembler.h"
#include "code_relocate.h"

namespace SandHook {
    namespace Hook {

        class InlineHook {
        public:
            //return == backup method
            virtual void* inlineHook(void* origin, void* replace) = 0;

        protected:
            static AndroidCodeBuffer* backupBuffer;
        public:
            static InlineHook* instance;
        };

    }
}

#endif //SANDHOOK_NH_HOOK_H
