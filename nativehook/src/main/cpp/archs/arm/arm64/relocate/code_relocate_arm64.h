//
// Created by swift on 2019/5/12.
//

#ifndef SANDHOOK_NH_CODE_RELOCATE_A64_H
#define SANDHOOK_NH_CODE_RELOCATE_A64_H

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

            void* relocate(Instruction<Base> *instruction, void *toPc) throw(ErrorCodeException) override;

            void* relocate(void *startPc, Addr len, void *toPc) throw(ErrorCodeException) override;

            bool visit(Unit<Base> *unit, void *pc) override;

            bool inRelocateRange(Off targetOffset, Addr targetLen);

            Label* getLaterBindLabel(Addr offset);

            DEFINE_RELOCATE(B_BL)

            DEFINE_RELOCATE(B_COND)

            DEFINE_RELOCATE(TBZ_TBNZ)

            DEFINE_RELOCATE(CBZ_CBNZ)

            DEFINE_RELOCATE(LDR_LIT)

            DEFINE_RELOCATE(ADR_ADRP)


            ~CodeRelocateA64() {
                delete relocateLock;
                delete laterBindlabels;
            }


        private:
            AssemblerA64* assemblerA64;
            std::mutex* relocateLock = new std::mutex();
            std::map<Addr, Label*>* laterBindlabels = new std::map<Addr, Label*>();
            Addr startAddr;
            Addr length;
            Addr curOffset;
        };

    }
}

#endif //SANDHOOK_NH_CODE_RELOCATE_A64_H
