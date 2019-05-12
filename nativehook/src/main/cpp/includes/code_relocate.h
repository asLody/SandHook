//
// Created by swift on 2019/5/10.
//

#ifndef SANDHOOK_NH_CODE_RELOCATE_H
#define SANDHOOK_NH_CODE_RELOCATE_H

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

            virtual bool relocate(Instruction<Base> *instruction, void* toPc) throw(ErrorCodeException) = 0;
            virtual bool relocate(void* startPc, void* toPc, Addr len) throw(ErrorCodeException) = 0;

        private:
            CodeContainer* codeContainer;
        };

    }
}

#endif //SANDHOOK_NH_CODE_RELOCATE_H
