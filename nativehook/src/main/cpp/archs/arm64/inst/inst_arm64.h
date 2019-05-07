//
// Created by swift on 2019/5/6.
//

#ifndef SANDHOOK_NH_INST_ARM64_H
#define SANDHOOK_NH_INST_ARM64_H

#include <register.h>
#include "inst_struct_aarch64.h"
#include "../../../asm/instruction.h"
#include "../../../includes/base.h"
#include "../register/register_a64.h"

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

        class A64_MOV_WIDE : public InstructionA64<aarch64_mov_wide> {
        public:

            enum MOV_WideOp {
                // Move and keep.
                MOV_WideOp_K = 0b00,
                // Move with zero.
                MOV_WideOp_Z = 0b10,
                // Move with non-zero.
                MOV_WideOp_N = 0b11,
            };

            A64_MOV_WIDE();

            A64_MOV_WIDE(aarch64_mov_wide *inst);

            A64_MOV_WIDE(A64_MOV_WIDE::MOV_WideOp op, RegisterA64* rd, U16 imme, U8 shift);

            void assembler() override;

            void decode(aarch64_mov_wide *decode) override;


            inline U8 getShift() {
                return shift;
            }

            inline MOV_WideOp getOpt() {
                return op;
            }

            inline U16 getImme() {
                return imme;
            }

            inline Register* getRd() {
                return rd;
            }

        private:
            //can be 16/32/64/128
            //hw = shift / 16
            U8 shift;
            MOV_WideOp op;
            U16 imme;
            RegisterA64* rd;
        };

    }

}

#endif //SANDHOOK_NH_INST_ARM64_H
