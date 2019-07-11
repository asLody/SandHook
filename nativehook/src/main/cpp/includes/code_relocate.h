//
// Created by swift on 2019/5/10.
//

#pragma once

#include <mutex>
#include <map>
#include "exception.h"
#include "instruction.h"
#include "assembler.h"
#include "decoder.h"

using namespace SandHook::Assembler;
using namespace SandHook::Decoder;

namespace SandHook {
    namespace Asm {

        class CodeRelocate : public InstVisitor {
        public:

            CodeRelocate(CodeContainer &codeContainer) : code_container(&codeContainer) {}

            virtual void* Relocate(BaseInst *instruction, void *toPc) throw(ErrorCodeException) = 0;
            virtual void* Relocate(void *startPc, Addr len, void *toPc) throw(ErrorCodeException) = 0;

            bool InRelocateRange(Off targetOffset, Addr targetLen);

            Label* GetLaterBindLabel(Addr offset);

            virtual ~CodeRelocate() {
                delete relocate_lock;
                delete later_bind_labels;
            }

        public:
            CodeContainer* code_container;
            std::mutex* relocate_lock = new std::mutex();
            std::map<Addr, Label*>* later_bind_labels = new std::map<Addr, Label*>();
            Addr start_addr = 0;
            Addr length = 0;
            Addr cur_offset = 0;
        };

    }
}