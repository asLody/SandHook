//
// Created by swift on 2019/5/23.
//

#pragma once

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
            void *Hook(void *origin, void *replace) override;

            bool BreakPoint(void *point, void (*callback)(REG *)) override;

        protected:
            std::mutex* hookLock;
        };

    }
}