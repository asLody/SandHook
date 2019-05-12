//
// Created by swift on 2019/5/6.
//

#ifndef SANDHOOK_NH_INST_ARM64_H
#define SANDHOOK_NH_INST_ARM64_H

#include "inst_struct_aarch64.h"
#include "instruction.h"
#include "base.h"
#include "register_list_a64.h"


#define INST_A64(X) A64_##X

#define IS_OPCODE(RAW,OP) INST_A64(OP)::is(RAW)

#define DEFINE_IS_EXT(X, COND) \
inline static bool is(InstA64& inst) { \
union { \
    InstA64 raw; \
    STRUCT_A64(X) inst; \
} inst_test; \
inst_test.raw = inst; \
return COND; \
}

#define DEFINE_IS(X) DEFINE_IS_EXT(X, TEST_INST_FIELD(opcode,OPCODE_A64(X)))

#define TEST_INST_FIELD(F,V) inst_test.inst.F == V

#define TEST_INST_OPCODE(X, INDEX) inst_test.inst.opcode##INDEX == OPCODE_A64(X##_##INDEX)

#define DEFINE_INST_CODE(X) \
inline U32 instCode() override { \
return InstCodeA64::X; \
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

            U32 size() override;

            static inline U32 zeroExtend32(unsigned int bits, U32 value) {
                return value << (32 - bits);
            }

            static inline S64 signExtend64(unsigned int bits, U64 value) {
                return ExtractSignedBitfield64(bits - 1, 0, value);
            }

            static inline S32 signExtend32(unsigned int bits, U32 value) {
                return ExtractSignedBitfield32(bits - 1, 0, value);
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

        enum AddrMode { Offset, PreIndex, PostIndex, NonAddrMode};

        enum MemOp {MemOp_LOAD, MemOp_STORE, MemOp_PREFETCH};

        enum FPTrapFlags {
            EnableTrap   = 1,
            DisableTrap = 0
        };

        enum FlagsUpdate {
            SetFlags   = 1,
            LeaveFlags = 0
        };

        enum ExceptionLevel {
            EL0 = 0b00,
            EL1 = 0b01,
            EL2 = 0b10,
            EL3 = 0b11
        };

        class Operand {
        public:
            inline explicit Operand(){};
            inline explicit Operand(S64 imm)
                    : immediate(imm), reg(&UnknowRegiser), shift(NO_SHIFT), extend(NO_EXTEND), shift_extend_imm(0) {}
            inline Operand(RegisterA64* reg, Shift shift = LSL, int32_t imm = 0)
                    : immediate(0), reg(reg), shift(shift), extend(NO_EXTEND), shift_extend_imm(imm) {}
            inline Operand(RegisterA64* reg, Extend extend, int32_t imm = 0)
                    : immediate(0), reg(reg), shift(NO_SHIFT), extend(extend), shift_extend_imm(imm) {}

            // =====

            bool IsImmediate() const { return reg->is(UnknowRegiser); }
            bool IsShiftedRegister() const { return  (shift != NO_SHIFT); }
            bool IsExtendedRegister() const { return (extend != NO_EXTEND); }

        public:
            S64 immediate;
            RegisterA64* reg;
            Shift shift;
            Extend extend;
            int32_t shift_extend_imm;
        };

        class MemOperand {
        public:
            inline explicit MemOperand() {}
            inline explicit MemOperand(RegisterA64* base, Off offset = 0, AddrMode addr_mode = Offset)
                    : base(base), reg_offset(&UnknowRegiser), offset(offset), addr_mode(addr_mode), shift(NO_SHIFT),
                      extend(NO_EXTEND), shift_extend_imm(0) {}

            inline explicit MemOperand(RegisterA64* base, RegisterA64* reg_offset, Extend extend, unsigned extend_imm)
                    : base(base), reg_offset(reg_offset), offset(0), addr_mode(Offset), shift(NO_SHIFT), extend(extend),
                      shift_extend_imm(extend_imm) {}

            inline explicit MemOperand(RegisterA64* base, RegisterA64* reg_offset, Shift shift = LSL, unsigned shift_imm = 0)
                    : base(base), reg_offset(reg_offset), offset(0), addr_mode(Offset), shift(shift), extend(NO_EXTEND),
                      shift_extend_imm(shift_imm) {}

            inline explicit MemOperand(RegisterA64* base, const Operand &offset, AddrMode addr_mode = Offset)
                    : base(base), reg_offset(&UnknowRegiser), addr_mode(addr_mode) {
                if (offset.IsShiftedRegister()) {
                    reg_offset        = offset.reg;
                    shift            = offset.shift;
                    shift_extend_imm = offset.shift_extend_imm;

                    extend = NO_EXTEND;
                    this->offset = 0;
                } else if (offset.IsExtendedRegister()) {
                    reg_offset        = offset.reg;
                    extend           = offset.extend;
                    shift_extend_imm = offset.shift_extend_imm;

                    shift  = NO_SHIFT;
                    this->offset = 0;
                }
            }

            // =====

            bool IsImmediateOffset() const { return (addr_mode == Offset); }
            bool IsRegisterOffset() const { return (addr_mode == Offset); }
            bool IsPreIndex() const { return addr_mode == PreIndex; }
            bool IsPostIndex() const { return addr_mode == PostIndex; }

        public:
            RegisterA64* base;
            RegisterA64* reg_offset;
            Off offset;
            AddrMode addr_mode;
            Shift shift;
            Extend extend;
            S32 shift_extend_imm;
        };


        class INST_A64(UNKNOW) : public InstructionA64<STRUCT_A64(UNKNOW)> {
        public:

            A64_UNKNOW(STRUCT_A64(UNKNOW) &inst);

            DEFINE_INST_CODE(UNKNOW)

            inline bool unknow() override {
                return true;
            }

            void decode(A64_STRUCT_UNKNOW *inst) override;

            void assembler() override;

        private:
            STRUCT_A64(UNKNOW) inst_backup;
        };


        template <typename Inst>
        class A64_INST_PC_REL : public InstructionA64<Inst> {
        public:

            A64_INST_PC_REL();

            A64_INST_PC_REL(Inst *inst);

            virtual Off getImmPCOffset() = 0;

            virtual Addr getImmPCOffsetTarget();

            bool pcRelate() override;

        };



        class INST_A64(ADR_ADRP) : public A64_INST_PC_REL<STRUCT_A64(ADR_ADRP)> {
        public:

            enum OP {
                ADR = 0b0,
                ADRP = 0b1,
            };

            A64_ADR_ADRP();

            A64_ADR_ADRP(STRUCT_A64(ADR_ADRP) &inst);

            A64_ADR_ADRP(OP op, XRegister &rd, S64 offset);

            A64_ADR_ADRP(OP op, XRegister &rd, Label &label);

            DEFINE_IS(ADR_ADRP)

            DEFINE_INST_CODE(ADR_ADRP)

            bool isADRP() {
                return get()->op == OP::ADRP;
            }

            Off getImmPCOffset() override;

            Addr getImmPCOffsetTarget() override;

            void decode(A64_STRUCT_ADR_ADRP *inst) override;

            void assembler() override;

        public:
            OP op;
            XRegister* rd;
            S64 offset;
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

            enum Shift {
                Shift0 = 0,
                Shift1 = 16,
                Shift2 = 32,
                Shift3 = 48
            };

            A64_MOV_WIDE();

            A64_MOV_WIDE(STRUCT_A64(MOV_WIDE) &inst);

            A64_MOV_WIDE(A64_MOV_WIDE::OP op, RegisterA64* rd, U16 imme, U8 shift);

            DEFINE_IS(MOV_WIDE)

            DEFINE_INST_CODE(MOV_WIDE)

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

        public:
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

            A64_B_BL(STRUCT_A64(B_BL) &inst);

            A64_B_BL(OP op, Off offset);

            A64_B_BL(OP op, Label &l);

            DEFINE_IS(B_BL)

            inline Off getOffset() {
                return offset;
            }

            inline OP getOP() {
                return op;
            }

            DEFINE_INST_CODE(B_BL)

            Off getImmPCOffset() override;

            void onOffsetApply(Off offset) override;

            void decode(STRUCT_A64(B_BL) *decode) override;

            void assembler() override;


        public:
            OP op;
            Off offset;
        };


        class INST_A64(CBZ_CBNZ) : public A64_INST_PC_REL<STRUCT_A64(CBZ_CBNZ)> {
        public:

            enum OP {
                CBZ = 0,
                CBNZ = 1
            };

            A64_CBZ_CBNZ();

            A64_CBZ_CBNZ(STRUCT_A64(CBZ_CBNZ) &inst);

            A64_CBZ_CBNZ(OP op, Off offset, RegisterA64 &rt);

            A64_CBZ_CBNZ(OP op, Label& label, RegisterA64 &rt);

            DEFINE_IS(CBZ_CBNZ)

            DEFINE_INST_CODE(CBZ_CBNZ)

            void onOffsetApply(Off offset) override;

            Off getImmPCOffset() override;

            void decode(STRUCT_A64(CBZ_CBNZ) *inst) override;

            void assembler() override;

        public:
            OP op;
            Off offset;
            RegisterA64* rt;
        };


        class INST_A64(B_COND) : public A64_INST_PC_REL<STRUCT_A64(B_COND)> {
        public:
            A64_B_COND();

            A64_B_COND(STRUCT_A64(B_COND) &inst);

            A64_B_COND(Condition condition, Off offset);

            A64_B_COND(Condition condition, Label& label);

            DEFINE_IS(B_COND)

            DEFINE_INST_CODE(B_COND)

            Off getImmPCOffset() override;

            void onOffsetApply(Off offset) override;

            void decode(STRUCT_A64(B_COND) *inst) override;

            void assembler() override;

        public:
            Condition condition;
            Off offset;
        };


        class INST_A64(TBZ_TBNZ) : public A64_INST_PC_REL<STRUCT_A64(TBZ_TBNZ)> {
        public:

            enum OP {
                TBZ = 0,
                TBNZ = 1
            };

            A64_TBZ_TBNZ();

            A64_TBZ_TBNZ(STRUCT_A64(TBZ_TBNZ) &inst);

            A64_TBZ_TBNZ(OP op, RegisterA64 &rt, U32 bit, Off offset);

            A64_TBZ_TBNZ(OP op, RegisterA64 &rt, U32 bit, Label& label);

            DEFINE_IS(TBZ_TBNZ)

            DEFINE_INST_CODE(TBZ_TBNZ)

            void onOffsetApply(Off offset) override;

            Off getImmPCOffset() override;

            void decode(STRUCT_A64(TBZ_TBNZ) *inst) override;

            void assembler() override;

        public:
            OP op;
            RegisterA64* rt;
            U32 bit;
            Off offset;
        };


        class INST_A64(LDR_LIT) : public A64_INST_PC_REL<STRUCT_A64(LDR_LIT)> {
        public:

            enum OP {
                LDR_W = 0b00,
                LDR_X = 0b01,
                LDR_SW = 0b10,
                LDR_PRFM = 0b11
            };

            A64_LDR_LIT();

            A64_LDR_LIT(STRUCT_A64(LDR_LIT) &inst);

            A64_LDR_LIT(OP op, RegisterA64 *rt, Off offset);

            DEFINE_IS(LDR_LIT)

            DEFINE_INST_CODE(LDR_LIT)

            Off getImmPCOffset() override;

            void decode(STRUCT_A64(LDR_LIT) *inst) override;

            void assembler() override;

        public:
            OP op;
            RegisterA64* rt;
            Off offset;
        };


        class INST_A64(BR_BLR_RET) : public InstructionA64<STRUCT_A64(BR_BLR_RET)> {
        public:

            enum OP {
                BR = 0b00,
                BLR = 0b01,
                RET = 0b11
            };

            A64_BR_BLR_RET();

            A64_BR_BLR_RET(STRUCT_A64(BR_BLR_RET) &inst);

            A64_BR_BLR_RET(OP op, XRegister &rn);

            DEFINE_IS_EXT(BR_BLR_RET, TEST_INST_OPCODE(BR_BLR_RET, 1) && TEST_INST_OPCODE(BR_BLR_RET, 2) && TEST_INST_OPCODE(BR_BLR_RET,3))

            DEFINE_INST_CODE(BR_BLR_RET)

            void decode(A64_STRUCT_BR_BLR_RET *inst) override;

            void assembler() override;

        public:
            OP op;
            XRegister* rn;
        };


        class INST_A64(STR_IMM) : public InstructionA64<STRUCT_A64(STR_IMM)> {
        public:

            enum AdMod {
                PostIndex = 0b00,
                PreIndex = 0b11
            };

            enum Size {
                Size32 = 0b10,
                Size64 = 0b11
            };

            A64_STR_IMM();

            A64_STR_IMM(STRUCT_A64(STR_IMM)& inst);

            A64_STR_IMM(RegisterA64 &rt, const MemOperand &operand);

            DEFINE_IS(STR_IMM)

            DEFINE_INST_CODE(MOV_REG)

            void decode(STRUCT_A64(STR_IMM) *inst) override;

            void assembler() override;

        public:
            RegisterA64* rt;
            MemOperand operand = MemOperand();
        private:
            AdMod addrMode;
            Size regSize;
            U8 scale;
            bool wback;
            bool postindex;
            Off offset;
        };


        class INST_A64(STR_UIMM) : public InstructionA64<STRUCT_A64(STR_UIMM)> {
        public:

            enum Size {
                Size32 = 0b10,
                Size64 = 0b11
            };

            A64_STR_UIMM();

            A64_STR_UIMM(STRUCT_A64(STR_UIMM)& inst);

            A64_STR_UIMM(RegisterA64 &rt, const MemOperand &operand);

            DEFINE_IS(STR_UIMM)

            DEFINE_INST_CODE(STR_UIMM)

            void decode(STRUCT_A64(STR_UIMM) *inst) override;

            void assembler() override;

        public:
            RegisterA64* rt;
            MemOperand operand = MemOperand();
        private:
            Size regSize;
            U8 scale;
            bool wback = false;
            bool postindex = false;
            Off offset;
        };


        class INST_A64(MOV_REG) : public InstructionA64<STRUCT_A64(MOV_REG)> {
        public:

            A64_MOV_REG();

            A64_MOV_REG(STRUCT_A64(MOV_REG) &inst);

            A64_MOV_REG(RegisterA64 &rd, RegisterA64 &rm);

            DEFINE_IS_EXT(MOV_REG, TEST_INST_OPCODE(MOV_REG, 1) && TEST_INST_OPCODE(MOV_REG, 2))

            DEFINE_INST_CODE(MOV_REG)

            void decode(A64_STRUCT_MOV_REG *inst) override;

            void assembler() override;

        public:
            RegisterA64* rd;
            RegisterA64* rm;
        };


        class INST_A64(SUB_EXT_REG) : InstructionA64<STRUCT_A64(SUB_EXT_REG)> {
        public:

            enum S {
                Unsign = 0b0,
                Sign = 0b1,
            };

            A64_SUB_EXT_REG();

            A64_SUB_EXT_REG(STRUCT_A64(SUB_EXT_REG) &inst);

            A64_SUB_EXT_REG(S s, RegisterA64 &rd, RegisterA64 &rn, const Operand &operand,
                             FlagsUpdate flagsUpdate);

            DEFINE_IS_EXT(SUB_EXT_REG, TEST_INST_OPCODE(SUB_EXT_REG, 1) && TEST_INST_OPCODE(SUB_EXT_REG, 2))

            DEFINE_INST_CODE(SUB_EXT_REG)

            void decode(A64_STRUCT_SUB_EXT_REG *inst) override;

            void assembler() override;


        public:
            S s;
            RegisterA64* rd;
            RegisterA64* rn;
            Operand operand = Operand();
            FlagsUpdate flagsUpdate;
        };


        class INST_A64(EXCEPTION_GEN) : public InstructionA64<STRUCT_A64(EXCEPTION_GEN)> {
        public:

            enum OP {
                XXC = 0b000,
                BRK = 0b001,
                HLT = 0b010,
                DCP = 0b101
            };

            A64_EXCEPTION_GEN();

            A64_EXCEPTION_GEN(STRUCT_A64(EXCEPTION_GEN) &inst);

            A64_EXCEPTION_GEN(OP op, ExceptionLevel el, U16 imme);

            DEFINE_IS_EXT(EXCEPTION_GEN, TEST_INST_OPCODE(EXCEPTION_GEN, 1) && TEST_INST_OPCODE(EXCEPTION_GEN, 2))

            DEFINE_INST_CODE(EXCEPTION_GEN)

            void decode(A64_STRUCT_EXCEPTION_GEN *inst) override;

            void assembler() override;

        public:
            OP op;
            ExceptionLevel el;
            U16 imme;
        };


        class INST_A64(SVC) : public INST_A64(EXCEPTION_GEN) {
        public:
            A64_SVC(U16 imme);

            A64_SVC();

            A64_SVC(STRUCT_A64(SVC) &inst);

            DEFINE_IS_EXT(EXCEPTION_GEN,  TEST_INST_OPCODE(EXCEPTION_GEN, 1) && TEST_INST_OPCODE(EXCEPTION_GEN, 2) && TEST_INST_FIELD(op, XXC) && TEST_INST_FIELD(ll, EL1))

            DEFINE_INST_CODE(SVC)
        };

    }

}

#endif //SANDHOOK_NH_INST_ARM64_H
