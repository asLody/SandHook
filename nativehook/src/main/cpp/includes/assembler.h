//
// Created by swift on 2019/5/7.
//

#pragma once

#include "label.h"
#include "instruction.h"
#include "data.h"
#include "platform.h"

namespace SandHook {

    using namespace Asm;

    namespace Assembler {

        class Assembler {

        };

        class CodeBuffer {
        public:
            virtual void* GetBuffer(U32 size) = 0;
            virtual void ResetLastBufferSize(U32 size){};
            virtual void* Copy(void* start, U32 size) {
                void* bufferStart = GetBuffer(size);
                if (bufferStart == nullptr)
                    return nullptr;
                memcpy(bufferStart, start, size);
                FlushCache((Addr) bufferStart, size);
                return bufferStart;
            };
        };

        class CodeContainer {
        public:

            CodeContainer(CodeBuffer *codeBuffer);

            void SetCodeBuffer(CodeBuffer *codeBuffer);

            //allow code Relocate to Get new pc first
            void AllocBufferFirst(U32 size);
            void Append(BaseUnit *unit);
            void Commit();

            Addr Size();

            virtual ~CodeContainer();

        public:
            //before commit is virtual address so = 0, after commit is real address
            Addr start_pc = 0;
            Addr cur_pc = 0;
        private:
            Addr max_size = 0;
            std::list<BaseUnit*> units = std::list<BaseUnit*>();
            std::list<Label*> labels = std::list<Label*>();
            CodeBuffer* code_buffer = nullptr;
        };


    }

}