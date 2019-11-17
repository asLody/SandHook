//
// Created by swift on 2019/5/23.
//

#pragma once

#include <mutex>
#include <map>
#include "code_relocate.h"
#include "assembler_arm32.h"

using namespace SandHook::Assembler;
using namespace SandHook::Decoder;
using namespace SandHook::AsmA32;

#define DEFINE_RELOCATE(T, X) void relocate_##T##_##X (INST_##T(X)* inst, void* toPc) throw(ErrorCodeException);

namespace SandHook {
    namespace Asm {

        class CodeRelocateA32 : public CodeRelocate {
        public:
            CodeRelocateA32(AssemblerA32 &assembler);

            void* Relocate(BaseInst *instruction, void *to_pc) throw(ErrorCodeException) override;

            void* Relocate(void *startPc, Addr len, void *toPc) throw(ErrorCodeException) override;

            bool Visit(BaseUnit *unit, void *pc) override;

            DEFINE_RELOCATE(T16, B_COND)

            DEFINE_RELOCATE(T16, B)

            DEFINE_RELOCATE(T16, BX_BLX)

            DEFINE_RELOCATE(T16, CBZ_CBNZ)

            DEFINE_RELOCATE(T16, LDR_LIT)

            DEFINE_RELOCATE(T16, ADR)

            DEFINE_RELOCATE(T16, ADD_REG_RDN)

            DEFINE_RELOCATE(T32, B32)

            DEFINE_RELOCATE(T32, LDR_LIT)


        private:
            AssemblerA32* assembler_a32;
        };

    }
}

#undef DEFINE_RELOCATE