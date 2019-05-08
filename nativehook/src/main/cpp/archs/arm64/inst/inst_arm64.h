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

            enum OP {
                ADR = 0b0,
                ADRP = 0b1,
            };

            A64_ADR_ADRP();

            A64_ADR_ADRP(aarch64_pcrel_insts *inst);

            A64_ADR_ADRP(OP op, RegisterA64 *rd, int imme);

            inline U32 instCode() override {
                return isADRP() ? PCRelAddressingOp::ADRP : PCRelAddressingOp::ADR;
            }

            inline bool isADRP() {
                return get()->op == OP::ADRP;
            }

            ADDR getImmPCOffset();

            ADDR getImmPCOffsetTarget();

            int getImm();

            void decode(aarch64_pcrel_insts *decode) override;

            void assembler() override;

        private:
            OP op;
            RegisterA64* rd;
            int imme;
        };



        class A64_MOV_WIDE : public InstructionA64<aarch64_mov_wide> {
        public:

            enum OP {
                // Move and keep.
                MOV_WideOp_K = 0b00,
                // Move with zero.
                MOV_WideOp_Z = 0b10,
                // Move with non-zero.
                MOV_WideOp_N = 0b11,
            };

            A64_MOV_WIDE();

            A64_MOV_WIDE(aarch64_mov_wide *inst);

            A64_MOV_WIDE(A64_MOV_WIDE::OP op, RegisterA64* rd, U16 imme, U8 shift);

            inline U32 instCode() override {
                switch (op) {
                    case MOV_WideOp_K:
                        return MOVK;
                    case MOV_WideOp_N:
                        return MOVN;
                    case MOV_WideOp_Z:
                        return MOVZ;
                    default:
                        return 0;
                }
            }

            void assembler() override;

            void decode(aarch64_mov_wide *decode) override;


            inline U8 getShift() {
                return shift;
            }

            inline OP getOpt() {
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
            OP op;
            U16 imme;
            RegisterA64* rd;
        };



        class A64_B_BL : public InstructionA64<aarch64_b_bl> {
        public:

            enum OP {
                B = 0b0,
                BL = 0b1
            };

            A64_B_BL();

            A64_B_BL(aarch64_b_bl *inst);

            inline U32 instCode() {
                return op == B ? UnconditionalBranchOp::B : UnconditionalBranchOp ::BL;
            };

        private:
            OP op;
            U32 imme;
        };

    }

}

#endif //SANDHOOK_NH_INST_ARM64_H
