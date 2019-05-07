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

            InstructionA64() {}

            InstructionA64(InstStruct *inst) : Instruction<InstStruct>(inst) {}

            InstStruct mask(InstStruct raw) {
                return raw & *(this->get());
            }

            bool IsPCRelAddressing() {
                return mask(PCRelAddressingFMask) == PCRelAddressingFixed;
            }

            InstType instType() {
                return A64;
            }

            Arch arch() {
                return arm64;
            }

            virtual int getImmBranch() {
                //TODO
                return 0;
            }

        };

        class A64_INST_PC_REL : public InstructionA64<aarch64_pcrel_insts> {
        public:

            A64_INST_PC_REL();

            A64_INST_PC_REL(aarch64_pcrel_insts *inst);

            virtual int getImmPCRel();

            virtual ADDR getImmPCOffset();

            virtual ADDR getImmPCOffsetTarget();

        };

        class A64_ADR_ADRP : public A64_INST_PC_REL {
        public:

            A64_ADR_ADRP();

            A64_ADR_ADRP(aarch64_pcrel_insts *inst);

            inline bool isADRP() {
                return get()->op == 1;
            }

            ADDR getImmPCOffset();

            ADDR getImmPCOffsetTarget();

            int getImm();

        };

    }

}

#endif //SANDHOOK_NH_INST_ARM64_H
