//
// Created by swift on 2019/5/6.
//

#pragma once

#include "inst_struct_aarch64.h"
#include "instruction.h"
#include "base.h"
#include "register_list_arm64.h"


#define INST_A64(X) A64_##X

#define IS_OPCODE_A64(RAW,OP) INST_A64(OP)::Is(RAW)

#define DEFINE_IS_EXT(X, COND) \
inline static bool Is(InstA64& inst) { \
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

#define DEFINE_INST(X) class INST_A64(X) : public InstructionA64<STRUCT_A64(X), ENUM_VALUE(InstCodeA64, InstCodeA64::X)>

#define DEFINE_INST_EXT(X,P) class INST_A64(X) : public INST_A64(P)

#define DEFINE_INST_EXT_(X,P) class INST_A64(X) : public P<STRUCT_A64(X), ENUM_VALUE(InstCodeA64, InstCodeA64::X)>

#define DEFINE_INST_PCREL(X) class INST_A64(X) : public A64_INST_PC_REL<STRUCT_A64(X), ENUM_VALUE(InstCodeA64, InstCodeA64::X)>

#define CAST_A64(X,V) reinterpret_cast<SandHook::AsmA64::INST_A64(X) *>(V)

using namespace SandHook::RegistersA64;
using namespace SandHook::Asm;

namespace SandHook {

    namespace AsmA64 {

        template<typename S, U32 C>
        class InstructionA64 : public Instruction<S, C> {
        public:

            InstructionA64() {}

            InstructionA64(void *inst) : Instruction<S, C>(inst) {}

            U32 Size() override {
                return 4;
            };

            InstType InstType() override {
                return A64;
            }

            Arch Arch() override {
                return arm64;
            }

        };

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
                    : immediate_(imm), reg_(&UnknowRegiser), shift_(NO_SHIFT), extend_(NO_EXTEND), shift_extend_imm_(0) {}
            inline explicit Operand(RegisterA64* reg, int32_t imm = 0, Shift shift = LSL)
                    : immediate_(0), reg_(reg), shift_(shift), extend_(NO_EXTEND), shift_extend_imm_(imm) {}
            inline explicit Operand(RegisterA64* reg, Extend extend, int32_t imm = 0)
                    : immediate_(0), reg_(reg), shift_(NO_SHIFT), extend_(extend), shift_extend_imm_(imm) {}

            // =====

            bool IsImmediate() const { return *reg_ == UnknowRegiser; }
            bool IsShiftedRegister() const { return  (shift_ != NO_SHIFT); }
            bool IsExtendedRegister() const { return (extend_ != NO_EXTEND); }

        public:
            S64 immediate_;
            RegisterA64* reg_;
            Shift shift_;
            Extend extend_;
            int32_t shift_extend_imm_;
        };

        class MemOperand {
        public:
            inline explicit MemOperand() {}
            inline explicit MemOperand(RegisterA64* base, Off offset = 0, AddrMode addr_mode = Offset)
                    : base_(base), reg_offset_(&UnknowRegiser), offset_(offset), addr_mode_(addr_mode), shift_(NO_SHIFT),
                      extend_(NO_EXTEND), shift_extend_imm_(0) {}

            inline explicit MemOperand(RegisterA64* base, RegisterA64* reg_offset, Extend extend, unsigned extend_imm)
                    : base_(base), reg_offset_(reg_offset), offset_(0), addr_mode_(Offset), shift_(NO_SHIFT), extend_(extend),
                      shift_extend_imm_(extend_imm) {}

            inline explicit MemOperand(RegisterA64* base, RegisterA64* reg_offset, Shift shift = LSL, unsigned shift_imm = 0)
                    : base_(base), reg_offset_(reg_offset), offset_(0), addr_mode_(Offset), shift_(shift), extend_(NO_EXTEND),
                      shift_extend_imm_(shift_imm) {}

            inline explicit MemOperand(RegisterA64* base, const Operand &offset, AddrMode addr_mode = Offset)
                    : base_(base), reg_offset_(&UnknowRegiser), addr_mode_(addr_mode) {
                if (offset.IsShiftedRegister()) {
                    reg_offset_        = offset.reg_;
                    shift_            = offset.shift_;
                    shift_extend_imm_ = offset.shift_extend_imm_;

                    extend_ = NO_EXTEND;
                    this->offset_ = 0;
                } else if (offset.IsExtendedRegister()) {
                    reg_offset_        = offset.reg_;
                    extend_           = offset.extend_;
                    shift_extend_imm_ = offset.shift_extend_imm_;

                    shift_  = NO_SHIFT;
                    this->offset_ = 0;
                }
            }

            // =====

            bool IsImmediateOffset() const { return (addr_mode_ == Offset); }
            bool IsRegisterOffset() const { return (addr_mode_ == Offset); }
            bool IsPreIndex() const { return addr_mode_ == PreIndex; }
            bool IsPostIndex() const { return addr_mode_ == PostIndex; }

        public:
            RegisterA64* base_;
            RegisterA64* reg_offset_;
            Off offset_;
            AddrMode addr_mode_;
            Shift shift_;
            Extend extend_;
            S32 shift_extend_imm_;
        };


        DEFINE_INST(UNKNOW) {
        public:

            A64_UNKNOW(void *inst) : InstructionA64(inst) {}

            INLINE bool Unknow() override {
                return true;
            }
        };


        template <typename S,U32 C>
        class A64_INST_PC_REL : public InstructionA64<S, C> {
        public:

            A64_INST_PC_REL();

            A64_INST_PC_REL(void *inst);

            virtual Off GetImmPCOffset() = 0;

            virtual Addr GetImmPCOffsetTarget();

            bool PcRelate() override;

        };



        DEFINE_INST_PCREL(ADR_ADRP) {
        public:

            enum OP {
                ADR = 0b0,
                ADRP = 0b1,
            };

            A64_ADR_ADRP(void *inst);

            A64_ADR_ADRP(OP op, XRegister &rd, S64 offset);

            A64_ADR_ADRP(OP op, XRegister &rd, Label *label);

            DEFINE_IS(ADR_ADRP)

            bool IsADRP() {
                return Get()->op == OP::ADRP;
            }

            Off GetImmPCOffset() override;

            Addr GetImmPCOffsetTarget() override;

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            XRegister* rd;
            S64 offset;
        };



        DEFINE_INST(MOV_WIDE) {
        public:

            enum OP {
                // Move and keep.
                MOV_WideOp_K = 0b11,
                // Move with zero.
                MOV_WideOp_Z = 0b10,
                // Move with non-zero.
                MOV_WideOp_N = 0b00,
            };

            enum Shift {
                Shift0 = 0,
                Shift1 = 16,
                Shift2 = 32,
                Shift3 = 48
            };

            A64_MOV_WIDE(void* inst);

            A64_MOV_WIDE(A64_MOV_WIDE::OP op, RegisterA64* rd, U16 imme, U8 shift);

            DEFINE_IS(MOV_WIDE)

            void Assemble() override;

            void Disassemble() override;

        public:
            //can be 16/32/64/128
            //hw = shift_ / 16
            U8 shift;
            OP op;
            U16 imme;
            RegisterA64* rd;
        };



        DEFINE_INST_PCREL(B_BL) {
        public:

            enum OP {
                B = 0b0,
                BL = 0b1
            };

            A64_B_BL(void *inst) ;

            A64_B_BL(OP op, Off offset);

            A64_B_BL(OP op, Label *l);

            DEFINE_IS(B_BL)

            inline Off getOffset() {
                return offset;
            }

            inline OP getOP() {
                return op;
            }

            Off GetImmPCOffset() override;

            void OnOffsetApply(Off offset) override;

            void Disassemble() override;

            void Assemble() override;


        public:
            OP op;
            Off offset;
        };


        DEFINE_INST_PCREL(CBZ_CBNZ) {
        public:

            enum OP {
                CBZ = 0,
                CBNZ = 1
            };

            A64_CBZ_CBNZ(void *inst);

            A64_CBZ_CBNZ(OP op, Off offset, RegisterA64 &rt);

            A64_CBZ_CBNZ(OP op, Label *label, RegisterA64 &rt);

            DEFINE_IS(CBZ_CBNZ)

            void OnOffsetApply(Off offset) override;

            Off GetImmPCOffset() override;

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            Off offset;
            RegisterA64* rt;
        };


        DEFINE_INST_PCREL(B_COND) {
        public:

            A64_B_COND(void *inst);

            A64_B_COND(Condition condition, Off offset);

            A64_B_COND(Condition condition, Label *label);

            DEFINE_IS(B_COND)

            Off GetImmPCOffset() override;

            void OnOffsetApply(Off offset) override;

            void Disassemble() override;

            void Assemble() override;

        public:
            Condition condition;
            Off offset;
        };


        DEFINE_INST_PCREL(TBZ_TBNZ) {
        public:

            enum OP {
                TBZ = 0,
                TBNZ = 1
            };


            A64_TBZ_TBNZ(void *inst);

            A64_TBZ_TBNZ(OP op, RegisterA64 &rt, U32 bit, Off offset);

            A64_TBZ_TBNZ(OP op, RegisterA64 &rt, U32 bit, Label* label);

            DEFINE_IS(TBZ_TBNZ)

            void OnOffsetApply(Off offset) override;

            Off GetImmPCOffset() override;

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            RegisterA64* rt;
            U32 bit;
            Off offset;
        };


        DEFINE_INST_PCREL(LDR_LIT) {
        public:

            enum OP {
                LDR_W = 0b00,
                LDR_X = 0b01,
                LDR_SW = 0b10,
                LDR_PRFM = 0b11
            };

            A64_LDR_LIT(void *inst);

            A64_LDR_LIT(OP op, RegisterA64 &rt, Off offset);

            A64_LDR_LIT(OP op, RegisterA64 &rt, Label* label);

            DEFINE_IS(LDR_LIT)

            Off GetImmPCOffset() override;

            void OnOffsetApply(Off offset) override;

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            RegisterA64* rt;
            Off offset;
        };


        DEFINE_INST(BR_BLR_RET) {
        public:

            enum OP {
                BR = 0b00,
                BLR = 0b01,
                RET = 0b11
            };

            A64_BR_BLR_RET(void *inst);

            A64_BR_BLR_RET(OP op, XRegister &rn);

            DEFINE_IS_EXT(BR_BLR_RET, TEST_INST_OPCODE(BR_BLR_RET, 1) && TEST_INST_OPCODE(BR_BLR_RET, 2) && TEST_INST_OPCODE(BR_BLR_RET,3))

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            XRegister* rn;
        };


        template <typename S, U32 C>
        class A64LoadAndStoreImm : public InstructionA64<S,C> {
        public:

            enum AdMod {
                Offset = 0b00,
                PostIndex = 0b01,
                PreIndex = 0b11
            };

            enum RegSize {
                Size32 = 0b10,
                Size64 = 0b11
            };

            A64LoadAndStoreImm(void *inst) : InstructionA64<S,C>(inst) {}

            A64LoadAndStoreImm(RegisterA64 *rt, const MemOperand &operand) : rt(rt),
                                                                             operand(operand) {}

            RegisterA64* rt;
            MemOperand operand = MemOperand();
        protected:
            Off offset;
            AdMod addrMode;
            RegSize regSize;
            U8 scale;
            bool wback;
            bool postindex;
        };


        DEFINE_INST_EXT_(STR_IMM, A64LoadAndStoreImm) {
        public:

            A64_STR_IMM(void *inst);

            A64_STR_IMM(RegisterA64 &rt, const MemOperand &operand);

            DEFINE_IS(STR_IMM)

            void Disassemble() override;

            void Assemble() override;

        };


        DEFINE_INST_EXT_(STR_UIMM, A64LoadAndStoreImm) {
        public:

            A64_STR_UIMM(void *inst);

            A64_STR_UIMM(RegisterA64 &rt, const MemOperand &operand);

            DEFINE_IS(STR_UIMM)

            void Disassemble() override;

            void Assemble() override;
        };


        DEFINE_INST(MOV_REG) {
        public:

            A64_MOV_REG(void *inst);

            A64_MOV_REG(RegisterA64 &rd, RegisterA64 &rm);

            DEFINE_IS_EXT(MOV_REG, TEST_INST_OPCODE(MOV_REG, 1) && TEST_INST_OPCODE(MOV_REG, 2))

            void Disassemble() override;

            void Assemble() override;

        public:
            RegisterA64* rd;
            RegisterA64* rm;
        };


        DEFINE_INST(SUB_EXT_REG) {
        public:

            A64_SUB_EXT_REG(void *inst);

            A64_SUB_EXT_REG(RegisterA64 &rd, RegisterA64 &rn, const Operand &operand,
                             FlagsUpdate flagsUpdate);

            DEFINE_IS_EXT(SUB_EXT_REG, TEST_INST_OPCODE(SUB_EXT_REG, 1) && TEST_INST_OPCODE(SUB_EXT_REG, 2))

            void Disassemble() override;

            void Assemble() override;


        public:
            RegisterA64* rd;
            RegisterA64* rn;
            Operand operand = Operand();
            FlagsUpdate flagsUpdate;
        };


        DEFINE_INST(EXCEPTION_GEN) {
        public:

            enum OP {
                XXC = 0b000,
                BRK = 0b001,
                HLT = 0b010,
                DCP = 0b101
            };

            A64_EXCEPTION_GEN(void *inst);

            A64_EXCEPTION_GEN(OP op, ExceptionLevel el, U16 imme);

            DEFINE_IS_EXT(EXCEPTION_GEN, TEST_INST_OPCODE(EXCEPTION_GEN, 1) && TEST_INST_OPCODE(EXCEPTION_GEN, 2))

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            ExceptionLevel el;
            U16 imme;
        };


        DEFINE_INST_EXT(SVC, EXCEPTION_GEN) {
        public:

            A64_SVC(U16 imme);

            A64_SVC(void *inst);

            DEFINE_IS_EXT(EXCEPTION_GEN,  TEST_INST_OPCODE(EXCEPTION_GEN, 1) && TEST_INST_OPCODE(EXCEPTION_GEN, 2) && TEST_INST_FIELD(op, XXC) && TEST_INST_FIELD(ll, EL1))

        };


        DEFINE_INST_EXT_(LDR_IMM, A64LoadAndStoreImm) {
        public:

            A64_LDR_IMM(void *inst);

            A64_LDR_IMM(RegisterA64 &rt, const MemOperand &operand);

            DEFINE_IS(LDR_IMM)

            void Disassemble() override;

            void Assemble() override;
        };


        DEFINE_INST_EXT_(LDR_UIMM, A64LoadAndStoreImm) {
        public:

            A64_LDR_UIMM(void *inst);

            A64_LDR_UIMM(RegisterA64 &rt, const MemOperand &operand);

            DEFINE_IS(LDR_UIMM)

            void Disassemble() override;

            void Assemble() override;
        };

        DEFINE_INST_EXT(LDRSW_IMM, LDR_IMM) {
        public:

            A64_LDRSW_IMM(void *inst);

            A64_LDRSW_IMM(RegisterA64 &rt, const MemOperand &operand);

            DEFINE_IS_EXT(LDRSW_IMM, TEST_INST_FIELD(opcode, OPCODE_A64(LDRSW_IMM)) &&
                    TEST_INST_FIELD(size , Size32))

            void Disassemble() override;

            void Assemble() override;
        };


        DEFINE_INST_EXT(LDRSW_UIMM, LDR_UIMM) {
        public:
            A64_LDRSW_UIMM();

            A64_LDRSW_UIMM(void *inst);

            A64_LDRSW_UIMM(XRegister &rt, const MemOperand &operand);

            DEFINE_IS_EXT(LDRSW_UIMM, TEST_INST_FIELD(opcode, OPCODE_A64(LDRSW_UIMM)) &&
            TEST_INST_FIELD(size, Size32))

            void Disassemble() override;

            void Assemble() override;
        };


        DEFINE_INST(STP_LDP) {
        public:
            enum OP {
                STP = 0b0,
                LDP = 0b1
            };

            enum AdMod {
                SignOffset = 0b10,
                PostIndex = 0b01,
                PreIndex = 0b11
            };

            enum RegSize {
                Size32 = 0b00,
                Size64 = 0b10
            };

            A64_STP_LDP(void *inst);

            A64_STP_LDP(OP op, RegisterA64 &rt1, RegisterA64 &rt2, const MemOperand &operand);

            DEFINE_IS(STP_LDP)

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            RegisterA64* rt1;
            RegisterA64* rt2;
            MemOperand operand;
        };


        DEFINE_INST(ADD_SUB_IMM) {
        public:

            enum OP {
                ADD = 0b0,
                SUB = 0b1
            };

            enum S {
                Sign = 0b1,
                UnSign = 0b0
            };

            enum RegSize {
                Size32 = 0b0,
                Size64 = 0b1
            };

            A64_ADD_SUB_IMM(void *inst);

            A64_ADD_SUB_IMM(OP op, S sign, RegisterA64 &rd, const Operand &operand);

            DEFINE_IS(ADD_SUB_IMM)

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            S sign;
            RegisterA64* rd;
            Operand operand;
        };


        DEFINE_INST(MSR_MRS) {
        public:

            enum OP {
                MSR = 0,
                MRS = 1
            };

            A64_MSR_MRS(void *inst);

            A64_MSR_MRS(OP op, SystemRegister &systemRegister, RegisterA64 &rt);

            DEFINE_IS(MSR_MRS)

            void Disassemble() override;

            void Assemble() override;

        public:
            OP op;
            SystemRegister system_reg{};
            RegisterA64* rt;
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
