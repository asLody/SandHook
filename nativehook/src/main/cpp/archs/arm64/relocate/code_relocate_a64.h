//
// Created by swift on 2019/5/12.
//

#ifndef SANDHOOK_NH_CODE_RELOCATE_A64_H
#define SANDHOOK_NH_CODE_RELOCATE_A64_H

#include "code_relocate.h"
#include "assembler_a64.h"

using namespace SandHook::Assembler;
using namespace SandHook::Decoder;

namespace SandHook {
    namespace Asm {

        class CodeRelocateA64 : public CodeRelocate {
        public:
            CodeRelocateA64(AssemblerA64 &assembler);

            bool relocate(Instruction<Base> *instruction, void *toPc) throw(ErrorCodeException) override;

            bool relocate(void *startPc, void *toPc, Addr len) throw(ErrorCodeException) override;

        private:
            AssemblerA64* assemblerA64;
        };

    }
}

#endif //SANDHOOK_NH_CODE_RELOCATE_A64_H
