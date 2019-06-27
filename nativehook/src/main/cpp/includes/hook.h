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

typedef Addr REG;

namespace SandHook {
    namespace Hook {

        class InlineHook {
        public:
            //return == backup method
            virtual void *Hook(void *origin, void *replace) = 0;
            virtual bool BreakPoint(void *point, void (*callback)(REG[])) {
                return false;
            };
        protected:
            static CodeBuffer* backupBuffer;
            std::mutex hookLock;
        public:
            static InlineHook* instance;
        };

    }
}

#endif //SANDHOOK_NH_HOOK_H
