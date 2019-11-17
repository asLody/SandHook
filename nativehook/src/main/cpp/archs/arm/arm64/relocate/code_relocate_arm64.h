//
// Created by swift on 2019/5/12.
//

#pragma once

#include <mutex>
#include <map>
#include "code_relocate.h"
#include "assembler_arm64.h"

using namespace SandHook::Assembler;
using namespace SandHook::Decoder;
using namespace SandHook::AsmA64;

#define DEFINE_RELOCATE(X) void relocate_##X (INST_A64(X)* inst, void* toPc) throw(ErrorCodeException);

namespace SandHook {
    namespace Asm {

        class CodeRelocateA64 : public CodeRelocate {
        public:
            CodeRelocateA64(AssemblerA64 &assembler);

            void* Relocate(BaseInst *instruction, void *toPc) throw(ErrorCodeException) override;

            void* Relocate(void *startPc, Addr len, void *toPc) throw(ErrorCodeException) override;

            bool Visit(BaseUnit *unit, void *pc) override;

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

#undef DEFINE_RELOCATE