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
#include "../../../../../../../hooklib/src/main/cpp/includes/inst.h"


#define INST_A64(X) A64_##X

#define IS_OPCODE(RAW,OP) INST_A64(OP)::is(RAW);


#define DEFINE_IS(X) \
inline static bool is(InstA64& inst) { \
union { \
    InstA64 raw; \
    STRUCT_A64(X) inst; \
} inst_test; \
inst_test.raw = inst; \
return inst_test.inst.opcode == OPCODE_A64(X); \
}

namespace SandHook {

    namespace Asm {

        template<typename Inst>
        class InstructionA64 : public Instruction<Inst> {
        public:

            InstructionA64() {}

            InstructionA64(Inst *inst) : Instruction<Inst>(inst) {}

            Inst mask(Inst raw) {
                return raw & *(this->get());
            }

            U8 size() override;

            static inline U32 signExtend64(unsigned int bits, U64 value) {
                U32 C = (U32) ((-1) << (bits - (U32) 1));
                return static_cast<U32>((value + C) ^ C);
            }

            bool isPCRelAddressing() {
                return mask(PCRelAddressingFMask) == PCRelAddressingFixed;
            }

            InstType instType() override {
                return A64;
            }

            Arch arch() override {
                return arm64;
            }

        };


        template <typename Inst>
        class A64_INST_PC_REL : public InstructionA64<Inst> {
        public:

            A64_INST_PC_REL();

            A64_INST_PC_REL(Inst *inst);

            virtual ADDR getImmPCOffset() = 0;

            virtual ADDR getImmPCOffsetTarget();

        };



        class INST_A64(ADR_ADRP) : public A64_INST_PC_REL<STRUCT_A64(ADR_ADRP)> {
        public:

            enum OP {
                ADR = 0b0,
                ADRP = 0b1,
            };

            A64_ADR_ADRP();

            A64_ADR_ADRP(STRUCT_A64(ADR_ADRP) *inst);

            A64_ADR_ADRP(OP op, RegisterA64 *rd, int imme);

            DEFINE_IS(ADR_ADRP)

            U32 instCode() override {
                return isADRP() ? PCRelAddressingOp::ADRP : PCRelAddressingOp::ADR;
            }

            bool isADRP() {
                return get()->op == OP::ADRP;
            }

            ADDR getImmPCOffset() override;

            ADDR getImmPCOffsetTarget() override;

            int getImm();

            void decode(STRUCT_A64(ADR_ADRP) *decode) override;

            void assembler() override;

        private:
            OP op;
            RegisterA64* rd;
            int imme;
        };



        class INST_A64(MOV_WIDE) : public InstructionA64<STRUCT_A64(MOV_WIDE)> {
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

            A64_MOV_WIDE(STRUCT_A64(MOV_WIDE) *inst);

            A64_MOV_WIDE(A64_MOV_WIDE::OP op, RegisterA64* rd, U16 imme, U8 shift);

            DEFINE_IS(MOV_WIDE)

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

            void decode(STRUCT_A64(MOV_WIDE) *decode) override;


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



        class INST_A64(B_BL) : public A64_INST_PC_REL<STRUCT_A64(B_BL)> {
        public:

            enum OP {
                B = 0b0,
                BL = 0b1
            };

            A64_B_BL();

            A64_B_BL(STRUCT_A64(B_BL) *inst);

            A64_B_BL(OP op, ADDR offset);

            inline ADDR getOffset() {
                return offset;
            }

            inline OP getOP() {
                return op;
            }

            inline U32 instCode() override {
                return op == B ? UnconditionalBranchOp::B : UnconditionalBranchOp::BL;
            };

            ADDR getImmPCOffset() override;

            void decode(STRUCT_A64(B_BL) *decode) override;

            void assembler() override;


        private:
            OP op;
            ADDR offset;
        };


        class INST_A64(CBZ_CBNZ) : public A64_INST_PC_REL<STRUCT_A64(CBZ_CBNZ)> {
        public:

            enum OP {
                CBZ = 0,
                CBNZ = 1
            };

            A64_CBZ_CBNZ();

            A64_CBZ_CBNZ(STRUCT_A64(CBZ_CBNZ) *inst);

            A64_CBZ_CBNZ(OP op, ADDR offset, RegisterA64 *rt);

            inline U32 instCode() override {
                return op == CBZ ? CompareBranchOp::CBZ : CompareBranchOp::CBNZ;
            }

            ADDR getImmPCOffset() override;

            void decode(STRUCT_A64(CBZ_CBNZ) *inst) override;

            void assembler() override;

        private:
            OP op;
            ADDR offset;
            RegisterA64* rt;
        };


        class INST_A64(B_COND) : public A64_INST_PC_REL<STRUCT_A64(B_COND)> {
        public:
            A64_B_COND();

            A64_B_COND(STRUCT_A64(B_COND) *inst);

            A64_B_COND(Condition condition, ADDR offset);

            inline U32 instCode() override {
                return B_cond;
            }

            ADDR getImmPCOffset() override;

            void decode(STRUCT_A64(B_COND) *inst) override;

            void assembler() override;

        private:
            Condition condition;
            ADDR offset;
        };


        class INST_A64(TBZ_TBNZ) : public A64_INST_PC_REL<STRUCT_A64(TBZ_TBNZ)> {
        public:

            enum OP {
                TBZ,
                TBNZ
            };

            A64_TBZ_TBNZ();

            A64_TBZ_TBNZ(STRUCT_A64(TBZ_TBNZ) *inst);

            A64_TBZ_TBNZ(OP op, RegisterA64 *rt, U32 bit, ADDR offset);

            inline U32 instCode() override {
                return op == TBZ ? TestBranchOp::TBZ : TestBranchOp::TBNZ;
            };

            ADDR getImmPCOffset() override;

            void decode(STRUCT_A64(TBZ_TBNZ) *inst) override;

            void assembler() override;

        private:
            OP op;
            RegisterA64* rt;
            U32 bit;
            ADDR offset;
        };

    }

}

#endif //SANDHOOK_NH_INST_ARM64_H
