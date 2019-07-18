//
// Created by swift on 2019/5/16.
//

#pragma once


#include "arm_base.h"
#include "arm32_base.h"
#include "register_list_arm32.h"
#include "inst_struct_t16.h"
#include "inst_code_arm32.h"
#include "inst_struct_t32.h"

#define INST_T32(X) T32_##X

#define IS_OPCODE_T32(RAW,OP) INST_T32(OP)::Is(RAW)

#define DEFINE_IS_EXT(X, COND) \
inline static bool Is(InstT32& inst) { \
union { \
    InstT32 raw; \
    STRUCT_T32(X) inst; \
} inst_test; \
inst_test.raw = inst; \
return COND; \
}

#define DEFINE_IS(X) DEFINE_IS_EXT(X, TEST_INST_FIELD(opcode,OPCODE_T32(X)))

#define TEST_INST_FIELD(F,V) inst_test.inst.F == V

#define INST_FIELD(F) inst_test.inst.F

#define TEST_INST_OPCODE(X, INDEX) inst_test.inst.opcode##INDEX == OPCODE_T32(X##_##INDEX)

#define DEFINE_INST(X) class INST_T32(X) : public InstructionT32<STRUCT_T32(X), ENUM_VALUE(InstCodeT32, InstCodeT32::X)>

#define DEFINE_INST_EXT(X,P) class INST_T32(X) : public INST_T32(P)

#define DEFINE_INST_EXT_(X,P) class INST_T32(X) : public P<STRUCT_T32(X), ENUM_VALUE(InstCodeT32, InstCodeT32::X)>

#define DEFINE_INST_PCREL(X) class INST_T32(X) : public T32_INST_PC_REL<STRUCT_T32(X), ENUM_VALUE(InstCodeT32, InstCodeT32::X)>


using namespace SandHook::RegistersA32;

namespace SandHook {

    namespace AsmA32 {

        template<typename S,U32 C>
        class InstructionT32 : public Instruction<S,C> {
        public:

            InstructionT32() {}

            InstructionT32(void *inst) : Instruction<S,C>(inst) {}

            U32 Size() override {
                return 4;
            }

            void *GetPC() override {
                return reinterpret_cast<void *>((Addr) Instruction<S,C>::GetPC() + 2 * 2);
            }

            Addr GetVPC() override {
                return Instruction<S,C>::GetVPC() + 2 * 2;
            }

            InstType InstType() override {
                return thumb32;
            }

            Arch Arch() override {
                return arm32;
            }

        };


        template <typename S,U32 C>
        class T32_INST_PC_REL : public InstructionT32<S,C> {
        public:

            T32_INST_PC_REL() {}

            T32_INST_PC_REL(void *inst) : InstructionT32<S,C>(inst) {};

            virtual Off GetImmPCOffset() {
                return 0;
            };

            virtual Addr GetImmPCOffsetTarget() {
                return (Addr) this->GetPC() + GetImmPCOffset();
            };

            inline bool PcRelate() override {
                return true;
            };

        };


        DEFINE_INST(UNKNOW) {
        public:

            T32_UNKNOW(void* inst);

            inline bool Unknow() override {
                return true;
            }

        };


        DEFINE_INST_PCREL(B32) {
        public:

            enum OP {
                B = 0b10,
                BL = 0b11
            };

            enum X {
                arm = 0b0,
                thumb = 0b1
            };

            T32_B32(void *inst);

            T32_B32(OP op, X x, Off offset);

            T32_B32(OP op, X x, Label* label);

            DEFINE_IS_EXT(B32, TEST_INST_FIELD(opcode, OPCODE_T32(B32)) && (TEST_INST_FIELD(op, B) || TEST_INST_FIELD(op, BL)))

            Addr GetImmPCOffsetTarget() override;

            Off GetImmPCOffset() override;

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            X x;
            Off offset;
        };

        DEFINE_INST(LDR_UIMM) {
        public:

            T32_LDR_UIMM(void *inst);

            T32_LDR_UIMM(RegisterA32 &rt, RegisterA32 &rn, U32 offset);

            DEFINE_IS(LDR_UIMM)

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterA32* rt;
            RegisterA32* rn;
            U32 offset;
        };


        DEFINE_INST_PCREL(LDR_LIT) {
        public:

            enum OP {
                LDR = 0b1011111,
                LDRB = 0b0011111,
                LDRH = 0b0111111
            };

            enum U {
                cmp = 0b0,
                add = 0b1
            };

            enum S {
                UnSign = 0b0,
                Sign = 0b1
            };

            T32_LDR_LIT(void *inst);

            T32_LDR_LIT(OP op, S s, RegisterA32 &rt, Off offset);

            T32_LDR_LIT(OP op, S s, RegisterA32 &rt, Label* label);

            DEFINE_IS_EXT(LDR_LIT, TEST_INST_FIELD(opcode, OPCODE_T32(LDR_LIT)) && (TEST_INST_FIELD(op, LDR) || TEST_INST_FIELD(op, LDRB) || TEST_INST_FIELD(op, LDRH)))

            Off GetImmPCOffset() override;

            Addr GetImmPCOffsetTarget() override;

            void OnOffsetApply(Off offset) override;

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            S s;
            RegisterA32* rt;
            Off offset;
        };


        DEFINE_INST(MOV_MOVT_IMM) {
        public:

            enum OP {
                MOV = 0b100100,
                MOVT = 0b101100
            };

            T32_MOV_MOVT_IMM(void *inst);

            T32_MOV_MOVT_IMM(OP op, RegisterA32 &rd, U16 imm16);

            DEFINE_IS_EXT(MOV_MOVT_IMM, TEST_INST_OPCODE(MOV_MOVT_IMM, 1) && TEST_INST_OPCODE(MOV_MOVT_IMM, 2) && (TEST_INST_FIELD(op, MOV) || TEST_INST_FIELD(op, MOVT)))

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            RegisterA32* rd;
            U16 imm16;
        };


        DEFINE_INST(LDR_IMM) {
        public:

            enum OP {
                LDR = 0b0101,
                LDRB = 0b1001,
                LDRH = 0b1011,
                LDRSB = 0b0001,
                LDRSH = 0b0011
            };

            T32_LDR_IMM(void *inst);

            T32_LDR_IMM(OP op, RegisterA32 &rt, const MemOperand &operand);

            DEFINE_IS_EXT(LDR_IMM, TEST_INST_OPCODE(LDR_IMM, 1) && TEST_INST_OPCODE(LDR_IMM, 2))

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            RegisterA32* rt;
            MemOperand operand;
        };


        DEFINE_INST(SUB_IMM) {
        public:

            enum OP {
                T3 = 0b01101,
                T4 = 0b10101
            };

            T32_SUB_IMM(void *inst);

            DEFINE_IS_EXT(SUB_IMM, TEST_INST_OPCODE(SUB_IMM, 1) && TEST_INST_OPCODE(SUB_IMM, 2) && (TEST_INST_FIELD(op, T3) || TEST_INST_FIELD(op, T4)))

        };


        DEFINE_INST(HVC) {
        public:
            T32_HVC(void *inst);

            T32_HVC(U16 imme);

            DEFINE_IS_EXT(HVC, TEST_INST_OPCODE(HVC, 1) && TEST_INST_OPCODE(HVC, 2))

            void Disassemble() override;

            void Assemble() override;

        public:
            U16 imme;
        };

    }
}


#undef DEFINE_IS_EXT
#undef DEFINE_IS
#undef TEST_INST_FIELD
#undef TEST_INST_OPCODE
#undef DEFINE_INST
#undef DEFINE_INST_EXT
#undef DEFINE_INST_EXT_
#undef DEFINE_INST_PCREL