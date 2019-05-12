//
// Created by swift on 2019/5/12.
//

#ifndef SANDHOOK_NH_CODE_RELOCATE_A64_H
#define SANDHOOK_NH_CODE_RELOCATE_A64_H

#include "code_relocate.h"
#include "assembler_a64.h"

using namespace SandHook::Assembler;
using namespace SandHook::Decoder;

#define DEFINE_RELOCATE(X) void relocate_##X (INST_A64(X)* inst, void* toPc) throw(ErrorCodeException);

namespace SandHook {
    namespace Asm {

        class CodeRelocateA64 : public CodeRelocate {
        public:
            CodeRelocateA64(AssemblerA64 &assembler);

            void relocate(Instruction<Base> *instruction, void *toPc) throw(ErrorCodeException) override;

            void relocate(void *startPc, void *toPc, Addr len) throw(ErrorCodeException) override;

            DEFINE_RELOCATE(B_BL)

            DEFINE_RELOCATE(B_COND)

            DEFINE_RELOCATE(TBZ_TBNZ)

            DEFINE_RELOCATE(CBZ_CBNZ)

            DEFINE_RELOCATE(LDR_LIT)

            DEFINE_RELOCATE(ADR_ADRP)


        private:
            AssemblerA64* assemblerA64;
        };

    }
}

#endif //SANDHOOK_NH_CODE_RELOCATE_A64_H
