//
// Created by swift on 2019/5/14.
//

#ifndef SANDHOOK_NH_HOOK_H
#define SANDHOOK_NH_HOOK_H

#include <mutex>
#include <hook.h>
#include <decoder.h>

#include "hook.h"
#include "assembler.h"
#include "code_relocate.h"

namespace SandHook {
    namespace Hook {

        class InlineHook {
        public:
            //return == backup method
            virtual void* inlineHook(void* origin, void* replace) = 0;
        };


        class InlineHookArm64Android : InlineHook {
        public:
            inline InlineHookArm64Android() {
                hookLock = new std::mutex();
            };
            inline ~InlineHookArm64Android() {
                delete hookLock;
            }
            void *inlineHook(void *origin, void *replace) override;
        protected:
            std::mutex* hookLock;
        };

    }
}

#endif //SANDHOOK_NH_HOOK_H
