//
// Created by swift on 2019/5/16.
//

#ifndef SANDHOOK_INST_T16_H
#define SANDHOOK_INST_T16_H

#include "arm_base.h"
#include "register_list_a32.h"
#include "inst_struct_t16.h"
#include "inst_code_arm32.h"

#define INST_T16(X) T16_##X

#define IS_OPCODE_T16(RAW,OP) INST_T16(OP)::is(RAW)

#define DEFINE_IS_EXT(X, COND) \
inline static bool is(InstT16& inst) { \
union { \
    InstT16 raw; \
    STRUCT_T16(X) inst; \
} inst_test; \
inst_test.raw = inst; \
return COND; \
}

#define DEFINE_IS(X) DEFINE_IS_EXT(X, TEST_INST_FIELD(opcode,OPCODE_T16(X)))

#define TEST_INST_FIELD(F,V) inst_test.inst.F == V

#define INST_FIELD(F) inst_test.inst.F

#define TEST_INST_OPCODE(X, INDEX) inst_test.inst.opcode##INDEX == OPCODE_T16(X##_##INDEX)

#define DEFINE_INST_CODE(X) \
inline U32 instCode() override { \
return InstCodeT16::X; \
}

using namespace SandHook::RegistersA32;

namespace SandHook {

    namespace AsmA32 {

        template<typename Inst>
        class InstructionT16 : public Instruction<Inst> {
        public:

            InstructionT16() {}

            InstructionT16(Inst *inst) : Instruction<Inst>(inst) {}

            Inst mask(Inst raw) {
                return raw & *(this->get());
            }

            U32 size() override {
                return 2;
            }

            static inline U32 zeroExtend32(unsigned int bits, U32 value) {
                return value << (32 - bits);
            }

            static inline S32 signExtend32(unsigned int bits, U32 value) {
                return ExtractSignedBitfield32(bits - 1, 0, value);
            }

            InstType instType() override {
                return thumb16;
            }

            Arch arch() override {
                return arm32;
            }

        };


        template <typename Inst>
        class T16_INST_PC_REL : public InstructionT16<Inst> {
        public:

            T16_INST_PC_REL() {};

            T16_INST_PC_REL(Inst *inst) : InstructionT16<Inst>(inst) {};

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


        class INST_T16(B) : public T16_INST_PC_REL<STRUCT_T16(B)> {
        public:
            T16_B();

            T16_B(STRUCT_T16(B) *inst);

            T16_B(Off offset);

            T16_B(Label& label);

            DEFINE_IS(B)

            DEFINE_INST_CODE(B)

            void onOffsetApply(Off offset) override;

            Off getImmPCOffset() override;

            void decode(STRUCT_T16(B) *inst) override;

            void assembler() override;

        public:
            Off offset;
        };

        class INST_T16(B_COND) : public T16_INST_PC_REL<STRUCT_T16(B_COND)> {
        public:
            T16_B_COND();

            T16_B_COND(STRUCT_T16(B_COND) *inst);

            T16_B_COND(Condition condition, Off offset);

            T16_B_COND(Condition condition, Label& label);

            DEFINE_IS_EXT(B_COND, TEST_INST_FIELD(opcode, OPCODE_T16(B_COND)) && INST_FIELD(cond) != 0b1110 && INST_FIELD(cond) != 0b1111)

            DEFINE_INST_CODE(B_COND)

            Off getImmPCOffset() override;

            void onOffsetApply(Off offset) override;

            void decode(STRUCT_T16(B_COND) *inst) override;

            void assembler() override;

        public:
            Condition condition;
            Off offset;
        };


        class INST_T16(BX_BLX) : T16_INST_PC_REL<STRUCT_T16(BX_BLX)> {
        public:

            enum OP {
                BX = 0b0,
                BLX = 0b1
            };

            T16_BX_BLX(T16_STRUCT_BX_BLX *inst);

            T16_BX_BLX(OP op, RegisterA32 &rm);

            DEFINE_IS_EXT(BX_BLX, TEST_INST_OPCODE(BX_BLX, 1) && TEST_INST_OPCODE(BX_BLX, 2))

            DEFINE_INST_CODE(BX_BLX)

            void decode(T16_STRUCT_BX_BLX *inst) override;

            void assembler() override;

        public:
            OP op;
            RegisterA32* rm;
        };


        class INST_T16(CBZ_CBNZ) : public T16_INST_PC_REL<STRUCT_T16(CBZ_CBNZ)> {
        public:
            enum OP {
                CBZ = 0b0,
                CBNZ = 0b1
            };

            T16_CBZ_CBNZ(T16_STRUCT_CBZ_CBNZ *inst);

            T16_CBZ_CBNZ(OP op, Off offset, RegisterA32 &rn);

            T16_CBZ_CBNZ(OP op, Label& label, RegisterA32 &rn);

            DEFINE_IS_EXT(CBZ_CBNZ, TEST_INST_OPCODE(CBZ_CBNZ, 1) && TEST_INST_OPCODE(CBZ_CBNZ, 2) && TEST_INST_OPCODE(CBZ_CBNZ, 3))

            DEFINE_INST_CODE(CBZ_CBNZ)

            void onOffsetApply(Off offset) override;

            Off getImmPCOffset() override;

            void decode(T16_STRUCT_CBZ_CBNZ *inst) override;

            void assembler() override;

        public:
            OP op;
            Off offset;
            RegisterA32* rn;
        };


        class INST_T16(LDR_LIT) : T16_INST_PC_REL<STRUCT_T16(LDR_LIT)> {
        public:
            T16_LDR_LIT(T16_STRUCT_LDR_LIT *inst);

            T16_LDR_LIT(Off offset, RegisterA32 &rt);

            DEFINE_IS(LDR_LIT)

            DEFINE_INST_CODE(LDR_LIT)

        private:
            Addr getImmPCOffsetTarget() override;

        public:

            Off getImmPCOffset() override;

            void onOffsetApply(Off offset) override;

            void decode(T16_STRUCT_LDR_LIT *inst) override;

            void assembler() override;

        public:
            Off offset;
            RegisterA32* rt;
        };

    }
}

#endif //SANDHOOK_INST_T16_H
