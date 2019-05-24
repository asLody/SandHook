//
// Created by swift on 2019/5/23.
//

#ifndef SANDHOOK_HOOK_ARM32_H
#define SANDHOOK_HOOK_ARM32_H

#include "hook.h"

namespace SandHook {
    namespace Hook {

        class InlineHookArm32Android : public InlineHook {
        public:
            inline InlineHookArm32Android() {
                hookLock = new std::mutex();
            };
            inline ~InlineHookArm32Android() {
                delete hookLock;
            }
            void *inlineHook(void *origin, void *replace) override;
        protected:
            std::mutex* hookLock;
        };

    }
}

#endif //SANDHOOK_HOOK_ARM32_H
