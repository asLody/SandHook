//
// Created by swift on 2019/5/16.
//

#pragma once

#include "arm_base.h"
#include "register_list_arm32.h"
#include "inst_struct_t16.h"
#include "inst_code_arm32.h"
#include "arm32_base.h"

#define INST_T16(X) T16_##X

#define IS_OPCODE_T16(RAW, OP) INST_T16(OP)::Is(RAW)

#define DEFINE_IS_EXT(X, COND) \
inline static bool Is(InstT16& inst) { \
union { \
    InstT16 raw; \
    STRUCT_T16(X) inst; \
} inst_test; \
inst_test.raw = inst; \
return COND; \
}

#define DEFINE_IS(X) DEFINE_IS_EXT(X, TEST_INST_FIELD(opcode,OPCODE_T16(X)))

#define TEST_INST_FIELD(F, V) inst_test.inst.F == V

#define INST_FIELD(F) inst_test.inst.F

#define TEST_INST_OPCODE(X, INDEX) inst_test.inst.opcode##INDEX == OPCODE_T16(X##_##INDEX)

#define DEFINE_INST(X) class INST_T16(X) : public InstructionT16<STRUCT_T16(X), ENUM_VALUE(InstCodeT16, InstCodeT16::X)>

#define DEFINE_INST_EXT(X,P) class INST_T16(X) : public INST_T16(P)

#define DEFINE_INST_EXT_(X,P) class INST_T16(X) : public P<STRUCT_T16(X), ENUM_VALUE(InstCodeT16, InstCodeT16::X)>

#define DEFINE_INST_PCREL(X) class INST_T16(X) : public T16_INST_PC_REL<STRUCT_T16(X), ENUM_VALUE(InstCodeT16, InstCodeT16::X)>


using namespace SandHook::RegistersA32;

namespace SandHook {

    namespace AsmA32 {

        template<typename S, U32 C>
        class InstructionT16 : public Instruction<S,C> {
        public:

            InstructionT16() {}

            InstructionT16(void *inst) : Instruction<S,C>(inst) {}

            inline U32 Size() override {
                return 2;
            }

            inline void *GetPC() override {
                return reinterpret_cast<void *>((Addr) Instruction<S,C>::GetPC() + 2 * 2);
            }

            inline Addr GetVPC() override {
                return Instruction<S,C>::GetVPC() + 2 * 2;
            }

            inline InstType InstType() override {
                return thumb16;
            }

            inline Arch Arch() override {
                return arm32;
            }

        };


        template<typename S,U32 C>
        class T16_INST_PC_REL : public InstructionT16<S,C> {
        public:

            T16_INST_PC_REL() {}

            T16_INST_PC_REL(void *inst) : InstructionT16<S,C>(inst) {};

            inline virtual Off GetImmPCOffset() {
                return 0;
            };

            inline virtual Addr GetImmPCOffsetTarget() {
                return (Addr) this->GetPC() + GetImmPCOffset();
            };

            inline bool PcRelate() override {
                return true;
            };

        };


        DEFINE_INST(UNKNOW) {
        public:

            T16_UNKNOW(void *inst);

            inline bool Unknow() override {
                return true;
            }

        };


        DEFINE_INST_PCREL(B) {
        public:

            T16_B(void *inst);

            T16_B(Off offset);

            T16_B(Label *label);

            DEFINE_IS(B)

            void OnOffsetApply(Off offset) override;

            Off GetImmPCOffset() override;

            void Disassemble() override;

            void Assemble() override;

        public:
            Off offset;
        };

        DEFINE_INST_PCREL(B_COND) {
        public:

            T16_B_COND(void *inst);

            T16_B_COND(Condition condition, Off offset);

            T16_B_COND(Condition condition, Label *label);

            DEFINE_IS_EXT(B_COND, TEST_INST_FIELD(opcode, OPCODE_T16(B_COND)) &&
                                  INST_FIELD(cond) != 0b1110 && INST_FIELD(cond) != 0b1111)

            Off GetImmPCOffset() override;

            void OnOffsetApply(Off offset) override;

            void Disassemble() override;

            void Assemble() override;

        public:
            Condition condition;
            Off offset;
        };


        DEFINE_INST_PCREL(BX_BLX) {
        public:

            enum OP {
                BX = 0b0,
                BLX = 0b1
            };

            T16_BX_BLX(void *inst);

            T16_BX_BLX(OP op, RegisterA32 &rm);

            DEFINE_IS_EXT(BX_BLX, TEST_INST_OPCODE(BX_BLX, 1) && TEST_INST_OPCODE(BX_BLX, 2))

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            RegisterA32 *rm;
        };


        DEFINE_INST_PCREL(CBZ_CBNZ) {
        public:
            enum OP {
                CBZ = 0b0,
                CBNZ = 0b1
            };

            T16_CBZ_CBNZ(void *inst);

            T16_CBZ_CBNZ(OP op, Off offset, RegisterA32 &rn);

            T16_CBZ_CBNZ(OP op, Label *label, RegisterA32 &rn);

            DEFINE_IS_EXT(CBZ_CBNZ,
                          TEST_INST_OPCODE(CBZ_CBNZ, 1) && TEST_INST_OPCODE(CBZ_CBNZ, 2) &&
                          TEST_INST_OPCODE(CBZ_CBNZ, 3))

            void OnOffsetApply(Off offset) override;

            Off GetImmPCOffset() override;

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            Off offset;
            RegisterA32 *rn;
        };


        DEFINE_INST_PCREL(LDR_LIT) {
        public:

            T16_LDR_LIT(void *inst);

            T16_LDR_LIT(Off offset, RegisterA32 &rt);

            DEFINE_IS(LDR_LIT)

            Addr GetImmPCOffsetTarget() override;

            Off GetImmPCOffset() override;

            void OnOffsetApply(Off offset) override;

            void Disassemble() override;

            void Assemble() override;

        public:
            Off offset;
            RegisterA32 *rt;
        };


        DEFINE_INST(ADD_IMM_RDN) {
        public:
            T16_ADD_IMM_RDN(void *inst);

            T16_ADD_IMM_RDN(RegisterA32 *rdn, U8 imm8);

            DEFINE_IS(ADD_IMM_RDN)

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterA32 *rdn;
            U8 imm8;
        };

        DEFINE_INST_PCREL(ADR) {
        public:

            T16_ADR(void *inst);

            T16_ADR(RegisterA32 &rd, Off offset);

            T16_ADR(RegisterA32 &rd, Label *label);

            DEFINE_IS(ADR)

            Off GetImmPCOffset() override;

            Addr GetImmPCOffsetTarget() override;

            void OnOffsetApply(Off offset) override;

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterA32 *rd;
            Off offset;
        };


        DEFINE_INST(CMP_REG) {
        public:
            T16_CMP_REG();

            T16_CMP_REG(void *inst);

            T16_CMP_REG(RegisterA32 &rm, RegisterA32 &rn);

            DEFINE_IS_EXT(CMP_REG, TEST_INST_FIELD(opcode_base, OPCODE_T16(DATA_PROC)) &&
                                   TEST_INST_FIELD(opcode, OPCODE_T16(CMP_REG)))

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterA32 *rm;
            RegisterA32 *rn;
        };


        DEFINE_INST(MOV_REG) {
        public:

            T16_MOV_REG(void *inst);

            T16_MOV_REG(RegisterA32 &rd, RegisterA32 &rm);

            DEFINE_IS_EXT(MOV_REG, TEST_INST_FIELD(opcode_base, OPCODE_T16(DATA_PROC)) &&
                                   TEST_INST_FIELD(opcode, OPCODE_T16(MOV_REG)))

            void Disassemble() override;

            void Assemble() override;

            bool PcRelate() override;

        public:
            RegisterA32 *rd;
            RegisterA32 *rm;
        };


        DEFINE_INST(ADD_REG) {
        public:

            T16_ADD_REG(void *inst);

            T16_ADD_REG(RegisterA32 *rd, RegisterA32 *rn, RegisterA32 *rm);

            DEFINE_IS(ADD_REG)

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterA32 *rd;
            RegisterA32 *rn;
            RegisterA32 *rm;
        };


        DEFINE_INST(CMP_REG_EXT) {
        public:

            T16_CMP_REG_EXT(void *inst);

            T16_CMP_REG_EXT(RegisterA32 &rn, RegisterA32 &rm);

            DEFINE_IS(CMP_REG_EXT)

            void Disassemble() override;

            void Assemble() override;


        public:
            RegisterA32 *rn;
            RegisterA32 *rm;
        };


        DEFINE_INST(POP) {
        public:
            T16_POP(void *inst);

            T16_POP(const RegisterList &registerList);

            DEFINE_IS(POP)

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterList registerList;
        };


        DEFINE_INST(PUSH) {
        public:

            T16_PUSH(void *inst);

            T16_PUSH(const RegisterList &registerList);

            DEFINE_IS(PUSH)

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterList registerList;
        };


        DEFINE_INST(ADD_REG_RDN) {
        public:
            T16_ADD_REG_RDN(void *inst);


            T16_ADD_REG_RDN(RegisterA32 &rdn, RegisterA32 &rm);


            DEFINE_IS(ADD_REG_RDN)

            bool PcRelate() override;

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterA32* rdn;
            RegisterA32* rm;
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