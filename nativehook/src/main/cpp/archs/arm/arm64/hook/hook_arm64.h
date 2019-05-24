//
// Created by swift on 2019/5/23.
//

#ifndef SANDHOOK_HOOK_ARM64_H
#define SANDHOOK_HOOK_ARM64_H

#include "hook.h"

namespace SandHook {
    namespace Hook {
        class InlineHookArm64Android : public InlineHook {
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

#endif //SANDHOOK_HOOK_ARM64_H
