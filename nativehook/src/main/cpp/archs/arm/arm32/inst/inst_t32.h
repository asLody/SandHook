//
// Created by swift on 2019/5/16.
//

#ifndef SANDHOOK_INST_T32_H
#define SANDHOOK_INST_T32_H

#include "arm_base.h"
#include "arm32_base.h"
#include "register_list_a32.h"
#include "inst_struct_t16.h"
#include "inst_code_arm32.h"
#include "inst_struct_t32.h"

#define INST_T32(X) T32_##X

#define IS_OPCODE_T32(RAW,OP) INST_T32(OP)::is(RAW)

#define DEFINE_IS_EXT(X, COND) \
inline static bool is(InstT16& inst) { \
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

#define DEFINE_INST_CODE(X) \
inline U32 instCode() override { \
return ENUM_VALUE(InstCodeT32, InstCodeT32::X); \
}

using namespace SandHook::RegistersA32;

namespace SandHook {

    namespace AsmA32 {

        template<typename Inst>
        class InstructionT32 : public Instruction<Inst> {
        public:

            InstructionT32() {}

            InstructionT32(Inst *inst) : Instruction<Inst>(inst) {}

            Inst mask(Inst raw) {
                return raw & *(this->get());
            }

            U32 size() override {
                return 4;
            }

            void onLabelApply(Addr pc) override {
                this->onOffsetApply(pc - this->getVPC() - 2 * size());
            }

            static inline S32 signExtend32(unsigned int bits, U32 value) {
                return ExtractSignedBitfield32(bits - 1, 0, value);
            }

            InstType instType() override {
                return thumb32;
            }

            Arch arch() override {
                return arm32;
            }

        };


        template <typename Inst>
        class T32_INST_PC_REL : public InstructionT32<Inst> {
        public:

            T32_INST_PC_REL() {};

            T32_INST_PC_REL(Inst *inst) : InstructionT32<Inst>(inst) {};

            virtual Off getImmPCOffset() {
                return 0;
            };

            virtual Addr getImmPCOffsetTarget() {
                return (Addr) this->getPC() + getImmPCOffset();
            };

            inline bool pcRelate() override {
                return true;
            };

        };


        class INST_T32(B32) : public T32_INST_PC_REL<STRUCT_T32(B32)> {
        public:

            enum OP {
                B = 0b10,
                BL = 0b11
            };

            enum X {
                arm = 0b0,
                thumb = 0b1
            };

            T32_B32(T32_STRUCT_B32 *inst);

            T32_B32(OP op, X x, Off offset);

            T32_B32(OP op, X x, Label& label);

            DEFINE_INST_CODE(B32)

            DEFINE_IS(B32)

            Off getImmPCOffset() override;

            void decode(T32_STRUCT_B32 *inst) override;

            void assembler() override;

        public:
            OP op;
            X x;
            Off offset;
        };

        class INST_T32(LDR_UIMM) : public InstructionT32<STRUCT_T32(LDR_UIMM)> {
        public:
            T32_LDR_UIMM(T32_STRUCT_LDR_UIMM *inst);

            T32_LDR_UIMM(RegisterA32 &rt, RegisterA32 &rn, U32 offset);

            DEFINE_INST_CODE(LDR_UIMM)

            DEFINE_IS(LDR_UIMM)

            void decode(T32_STRUCT_LDR_UIMM *inst) override;

            void assembler() override;

        public:
            RegisterA32* rt;
            RegisterA32* rn;
            U32 offset;
        };


        class INST_T32(LDR_LIT) : public T32_INST_PC_REL<STRUCT_T32(LDR_LIT)> {
        public:

            enum OP {
                LDR = 0b1011111,
                LDRB = 0b0011111,
                LDRH = 0b0111111
            };

            enum S {
                cmp = 0,
                add = 1
            };

            T32_LDR_LIT();

            T32_LDR_LIT(T32_STRUCT_LDR_LIT *inst);

            T32_LDR_LIT(OP op, RegisterA32 &rt, Off offset);

            T32_LDR_LIT(OP op, RegisterA32 &rt, Label& label);

            DEFINE_IS(LDR_LIT)

            DEFINE_INST_CODE(LDR_LIT)

            Off getImmPCOffset() override;

            void decode(T32_STRUCT_LDR_LIT *inst) override;

            void assembler() override;

        public:
            OP op;
            RegisterA32* rt;
            Off offset;
        };


        class INST_T32(MOV_MOVT_IMM) : public InstructionT32<STRUCT_T32(MOV_MOVT_IMM)> {
        public:

            enum OP {
                MOV = 0b100100,
                MOVT = 0b101100
            };

            T32_MOV_MOVT_IMM();

            T32_MOV_MOVT_IMM(T32_STRUCT_MOV_MOVT_IMM *inst);

            T32_MOV_MOVT_IMM(OP op, RegisterA32 &rd, U16 imm16);

            DEFINE_IS_EXT(MOV_MOVT_IMM, TEST_INST_OPCODE(MOV_MOVT_IMM, 1) && TEST_INST_OPCODE(MOV_MOVT_IMM, 2))

            DEFINE_INST_CODE(MOV_MOVT_IMM)

            void decode(T32_STRUCT_MOV_MOVT_IMM *inst) override;

            void assembler() override;

        public:
            OP op;
            RegisterA32* rd;
            U16 imm16;
        };


        class INST_T32(LDR_IMM) : public InstructionT32<STRUCT_T32(LDR_IMM)> {
        public:

            enum OP {
                LDR = 0b0101,
                LDRB = 0b1001,
                LDRH = 0b1011,
                LDRSB = 0b0001,
                LDRSH = 0b0011
            };

            T32_LDR_IMM(T32_STRUCT_LDR_IMM *inst);

            T32_LDR_IMM(OP op, RegisterA32 &rt, const MemOperand &operand);

            DEFINE_IS_EXT(LDR_IMM, TEST_INST_OPCODE(LDR_IMM, 1) && TEST_INST_OPCODE(LDR_IMM, 2))

            DEFINE_INST_CODE(LDR_IMM)

            void decode(T32_STRUCT_LDR_IMM *inst) override;

            void assembler() override;

        public:
            OP op;
            RegisterA32* rt;
            MemOperand operand;
        };

    }
}


#undef DEFINE_IS_EXT
#undef DEFINE_IS
#undef TEST_INST_FIELD
#undef TEST_INST_OPCODE
#undef DEFINE_INST_CODE

#endif //SANDHOOK_INST_T32_H
