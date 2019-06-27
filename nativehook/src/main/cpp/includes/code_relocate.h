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

            CodeRelocate(CodeContainer &codeContainer) : codeContainer(&codeContainer) {}

            virtual void* relocate(BaseInst *instruction, void* toPc) throw(ErrorCodeException) = 0;
            virtual void* relocate(void *startPc, Addr len, void *toPc) throw(ErrorCodeException) = 0;

            bool inRelocateRange(Off targetOffset, Addr targetLen);

            Label* getLaterBindLabel(Addr offset);

            virtual ~CodeRelocate() {
                delete relocateLock;
                delete laterBindlabels;
            }

        public:
            CodeContainer* codeContainer;
            std::mutex* relocateLock = new std::mutex();
            std::map<Addr, Label*>* laterBindlabels = new std::map<Addr, Label*>();
            Addr startAddr = 0;
            Addr length = 0;
            Addr curOffset = 0;
        };

    }
}