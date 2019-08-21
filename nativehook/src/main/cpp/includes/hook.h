//
// Created by swift on 2019/5/14.
//

#pragma once

#include <mutex>
#include <atomic>

#include "code_buffer.h"
#include "decoder.h"
#include "assembler.h"
#include "code_relocate.h"

typedef Addr REG;

namespace SandHook {
    namespace Hook {

        struct HookInfo {
            void *origin;
            void *replace;
            void *backup;
        };

        class InlineHook {
        public:
            //return == backup method
            virtual void *Hook(void *origin, void *replace) = 0;
            virtual bool BreakPoint(void *point, void (*callback)(REG[])) {
                return false;
            };
            virtual void *SingleInstHook(void *origin, void *replace) {
                return nullptr;
            };
            virtual void ExceptionHandler(int num, sigcontext *context) {};
        protected:

            virtual bool InitForSingleInstHook();

            bool inited = false;
            static CodeBuffer* backup_buffer;
            std::mutex hook_lock;
        public:
            static InlineHook* instance;
            void (*callback)(void) = nullptr;
        };

    }
}