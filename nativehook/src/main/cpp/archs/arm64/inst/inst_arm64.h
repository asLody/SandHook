//
// Created by swift on 2019/5/6.
//

#ifndef SANDHOOK_NH_INST_ARM64_H
#define SANDHOOK_NH_INST_ARM64_H

#include "inst_struct_aarch64.h"
#include "../../../asm/instruction.h"
#include "../../../includes/base.h"
#include "../register/register_list_a64.h"


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

            static inline U32 extend32(unsigned int bits, U32 value) {
                return value << (32 - bits);
            }

            static inline U64 signExtend64(unsigned int bits, U64 value) {
                U64 C = (U64) ((-1) << (bits - (U64) 1));
                return static_cast<U64>((value + C) ^ C);
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

        enum AddrMode { Offset, PreIndex, PostIndex };

        class Operand {
        public:
            inline explicit Operand(S64 imm)
                    : immediate_(imm), reg_(&UnknowRegiser), shift_(NO_SHIFT), extend_(NO_EXTEND), shift_extent_imm_(0) {}
            inline Operand(RegisterA64* reg, Shift shift = LSL, int32_t imm = 0)
                    : immediate_(0), reg_(reg), shift_(shift), extend_(NO_EXTEND), shift_extent_imm_(imm) {}
            inline Operand(RegisterA64* reg, Extend extend, int32_t imm = 0)
                    : immediate_(0), reg_(reg), shift_(NO_SHIFT), extend_(extend), shift_extent_imm_(imm) {}

            // =====

            bool IsImmediate() const { return reg_->is(UnknowRegiser); }
            bool IsShiftedRegister() const { return /* reg_.IsValid() && */ (shift_ != NO_SHIFT); }
            bool IsExtendedRegister() const { return /* reg_.IsValid() && */ (extend_ != NO_EXTEND); }

            // =====

            RegisterA64* reg() const {
                return reg_;
            }
            S64 Immediate() const { return immediate_; }
            Shift shift() const {
                return shift_;
            }
            Extend extend() const {
                return extend_;
            }
            int32_t shift_extend_imm() const { return shift_extent_imm_; }

        private:

            S64 immediate_;
            RegisterA64* reg_;
            Shift shift_;
            Extend extend_;
            int32_t shift_extent_imm_;
        };

        class MemOperand {
        public:
            inline explicit MemOperand(RegisterA64* base, int64_t offset = 0, AddrMode addrmode = Offset)
                    : base_(base), regoffset_(&UnknowRegiser), offset_(offset), addrmode_(addrmode), shift_(NO_SHIFT),
                      extend_(NO_EXTEND), shift_extend_imm_(0) {}

            inline explicit MemOperand(RegisterA64* base, RegisterA64* regoffset, Extend extend, unsigned extend_imm)
                    : base_(base), regoffset_(regoffset), offset_(0), addrmode_(Offset), shift_(NO_SHIFT), extend_(extend),
                      shift_extend_imm_(extend_imm) {}

            inline explicit MemOperand(RegisterA64* base, RegisterA64* regoffset, Shift shift = LSL, unsigned shift_imm = 0)
                    : base_(base), regoffset_(regoffset), offset_(0), addrmode_(Offset), shift_(shift), extend_(NO_EXTEND),
                      shift_extend_imm_(shift_imm) {}

            inline explicit MemOperand(RegisterA64* base, const Operand &offset, AddrMode addrmode = Offset)
                    : base_(base), regoffset_(&UnknowRegiser), addrmode_(addrmode) {
                if (offset.IsShiftedRegister()) {
                    regoffset_        = offset.reg();
                    shift_            = offset.shift();
                    shift_extend_imm_ = offset.shift_extend_imm();

                    extend_ = NO_EXTEND;
                    offset_ = 0;
                } else if (offset.IsExtendedRegister()) {
                    regoffset_        = offset.reg();
                    extend_           = offset.extend();
                    shift_extend_imm_ = offset.shift_extend_imm();

                    shift_  = NO_SHIFT;
                    offset_ = 0;
                }
            }

            const RegisterA64* base() const { return base_; }
            const RegisterA64* regoffset() const { return regoffset_; }
            int64_t offset() const { return offset_; }
            AddrMode addrmode() const { return addrmode_; }
            Shift shift() const { return shift_; }
            Extend extend() const { return extend_; }
            unsigned shift_extend_imm() const { return shift_extend_imm_; }

            // =====

            bool IsImmediateOffset() const { return (addrmode_ == Offset); }
            bool IsRegisterOffset() const { return (addrmode_ == Offset); }
            bool IsPreIndex() const { return addrmode_ == PreIndex; }
            bool IsPostIndex() const { return addrmode_ == PostIndex; }

        private:
            RegisterA64* base_;
            RegisterA64* regoffset_;
            int64_t offset_;
            AddrMode addrmode_;
            Shift shift_;
            Extend extend_;
            int32_t shift_extend_imm_;
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

        public:
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

            A64_B_BL(STRUCT_A64(B_BL) *inst);

            A64_B_BL(OP op, ADDR offset);

            DEFINE_IS(B_BL)

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


        public:
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

            DEFINE_IS(CBZ_CBNZ)

            inline U32 instCode() override {
                return op == CBZ ? CompareBranchOp::CBZ : CompareBranchOp::CBNZ;
            }

            ADDR getImmPCOffset() override;

            void decode(STRUCT_A64(CBZ_CBNZ) *inst) override;

            void assembler() override;

        public:
            OP op;
            ADDR offset;
            RegisterA64* rt;
        };


        class INST_A64(B_COND) : public A64_INST_PC_REL<STRUCT_A64(B_COND)> {
        public:
            A64_B_COND();

            A64_B_COND(STRUCT_A64(B_COND) *inst);

            A64_B_COND(Condition condition, ADDR offset);

            DEFINE_IS(B_COND)

            inline U32 instCode() override {
                return B_cond;
            }

            ADDR getImmPCOffset() override;

            void decode(STRUCT_A64(B_COND) *inst) override;

            void assembler() override;

        public:
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

            DEFINE_IS(TBZ_TBNZ)

            inline U32 instCode() override {
                return op == TBZ ? TestBranchOp::TBZ : TestBranchOp::TBNZ;
            };

            ADDR getImmPCOffset() override;

            void decode(STRUCT_A64(TBZ_TBNZ) *inst) override;

            void assembler() override;

        public:
            OP op;
            RegisterA64* rt;
            U32 bit;
            ADDR offset;
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

            A64_LDR_LIT(STRUCT_A64(LDR_LIT) *inst);

            A64_LDR_LIT(OP op, RegisterA64 *rt, ADDR offset);

            DEFINE_IS(LDR_LIT)

            inline U32 instCode() override {
                switch (op) {
                    case LDR_W:
                        return LoadLiteralOp::LDR_w_lit;
                    case LDR_X:
                        return LoadLiteralOp::LDR_x_lit;
                    case LDR_SW:
                        return LoadLiteralOp::LDRSW_x_lit;
                    case LDR_PRFM:
                        return LoadLiteralOp::PRFM_lit;
                }
            }

            ADDR getImmPCOffset() override;

            void decode(STRUCT_A64(LDR_LIT) *inst) override;

            void assembler() override;

        public:
            OP op;
            RegisterA64* rt;
            ADDR offset;
        };



        class INST_A64(STR_IMM) : public InstructionA64<STRUCT_A64(STR_IMM)> {
        public:
            A64_STR_IMM();

            A64_STR_IMM(A64_STRUCT_STR_IMM *inst);

            A64_STR_IMM(RegisterA64 *rt, const MemOperand &oprand);

        public:
            RegisterA64* rt;
            MemOperand oprand = MemOperand(nullptr);
        };

    }

}

#endif //SANDHOOK_NH_INST_ARM64_H
