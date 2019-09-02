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
            bool is_break_point;
            void *user_data;
            void *origin;
            void *replace;
            void *backup;
        };

        using BreakCallback = bool (*)(sigcontext*, void*);

        class InlineHook {
        public:
            //return == backup method
            virtual void *Hook(void *origin, void *replace) = 0;
            virtual bool BreakPoint(void *point, void (*callback)(REG[])) {
                return false;
            };
            virtual bool SingleBreakPoint(void *point, BreakCallback callback, void *data = nullptr) {
                return false;
            };
            virtual void *SingleInstHook(void *origin, void *replace) {
                return nullptr;
            };
            virtual bool ExceptionHandler(int num, sigcontext *context) {
                return false;
            };
        protected:

            virtual bool InitForSingleInstHook();

            bool inited = false;
            static CodeBuffer* backup_buffer;
            std::mutex hook_lock;

        private:
            using SigAct = int (*)(int, struct sigaction *, struct sigaction *);
            SigAct sigaction_backup = nullptr;
        public:
            static InlineHook* instance;
            struct sigaction old_sig_act{};
        };

    }
}