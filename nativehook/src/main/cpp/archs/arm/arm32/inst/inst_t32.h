//
// Created by swift on 2019/5/16.
//

#ifndef SANDHOOK_INST_T32_H
#define SANDHOOK_INST_T32_H

#include "arm_base.h"
#include "register_list_a32.h"
#include "inst_struct_t16.h"
#include "inst_code_arm32.h"
#include "inst_struct_t32.h"

#define INST_T32(X) T32_##X

#define IS_OPCODE_T16(RAW,OP) INST_T32(OP)::is(RAW)

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
return InstCodeT32::X; \
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

            Off getImmPCOffset() override;

            void decode(T32_STRUCT_B32 *inst) override;

            void assembler() override;

        public:
            OP op;
            X x;
            Off offset;
        };

    }
}

#endif //SANDHOOK_INST_T32_H
