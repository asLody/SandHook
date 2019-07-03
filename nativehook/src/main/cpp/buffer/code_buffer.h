//
// Created by swift on 2019/5/11.
//

#pragma once

#include <mutex>
#include "assembler.h"

namespace SandHook {
    namespace Assembler {
        class AndroidCodeBuffer : public CodeBuffer {
        public:

            AndroidCodeBuffer();

            void *GetBuffer(U32 bufferSize) override;

        protected:
            std::list<void*> execute_space_list = std::list<void*>();
            std::mutex alloc_space_lock;
            Addr execute_page_offset = 0;
            U32 current_execute_page_size = 0;
        };

        //thread unsafe
        class AndroidReSizableBufferUnsafe : public AndroidCodeBuffer {
        public:
        public:
            void ResetLastBufferSize(U32 size) override;

            void *GetBuffer(U32 bufferSize) override;

        private:
            U32 last_alloc_size;
        };

        class StaticCodeBuffer : public CodeBuffer {
        public:

            StaticCodeBuffer(Addr pc);

            void *GetBuffer(U32 bufferSize) override;

        private:
            Addr pc;
        };
    }
}
