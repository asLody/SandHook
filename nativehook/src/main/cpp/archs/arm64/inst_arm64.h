//
// Created by swift on 2019/5/6.
//

#ifndef SANDHOOK_NH_INST_ARM64_H
#define SANDHOOK_NH_INST_ARM64_H

#include "inst_struct_aarch64.h"
#include "../../asm/instruction.h"
#include "../../includes/base.h"

namespace SandHook {

    namespace Asm {

        template<typename InstStruct>
        class InstructionA64 : public Instruction<InstStruct> {
        public:

            inline InstStruct mask(InstStruct raw) {
                return raw & (InstStruct) *this;
            }

            inline bool IsPCRelAddressing() {
                return mask(PCRelAddressingFMask) == PCRelAddressingFixed;
            }

            inline InstType instType() override {
                return A64;
            }

            inline Arch arch() override {
                return arm64;
            }

            int getImmBranch() {
                //TODO
                return 0;
            }

        };

        class A64_INST_PC_REL : public InstructionA64<aarch64_pcrel_insts> {
        public:

            int getImmPCRel();

            ADDR getImmPCOffset();

            ADDR getImmPCOffsetTarget();

        };

        class A64_ADR_ADRP : public A64_INST_PC_REL {
        public:

            inline bool isADRP() {
                return get().op == 1;
            }

            ADDR getImmPCOffset();

            ADDR getImmPCOffsetTarget();

            int getImm();

        };

    }

}

#endif //SANDHOOK_NH_INST_ARM64_H
