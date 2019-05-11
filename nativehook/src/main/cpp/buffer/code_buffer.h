//
// Created by swift on 2019/5/11.
//

#ifndef SANDHOOK_NH_CODE_BUFFER_H
#define SANDHOOK_NH_CODE_BUFFER_H

#include <mutex>
#include "assembler.h"

namespace SandHook {
    namespace Assembler {
        class AndroidCodeBuffer : public CodeBuffer {
        public:
            void *getBuffer(U32 bufferSize) override;

        private:
            std::list<void*> executeSpaceList = std::list<void*>();
            std::mutex allocSpaceLock;
            Addr executePageOffset = 0;
            U32 currentExecutePageSize = 0;
        };

        class StaticCodeBuffer : public CodeBuffer {
        public:

            StaticCodeBuffer(Addr pc);

            void *getBuffer(U32 bufferSize) override;

        private:
            Addr pc;
        };
    }
}

#endif //SANDHOOK_NH_CODE_BUFFER_H
