//
// Created by swift on 2019/5/6.
//

#include "inst_arm64.h"

#define SET_OPCODE(X) get()->opcode = OPCODE_A64(X)

using namespace SandHook::Asm;

template<typename InstStruct>
U32 InstructionA64<InstStruct>::size() {
    return sizeof(InstA64);
}

//PC Rel Inst

template<typename Inst>
A64_INST_PC_REL<Inst>::A64_INST_PC_REL(Inst *inst):InstructionA64<Inst>(inst) {
}

template<typename Inst>
A64_INST_PC_REL<Inst>::A64_INST_PC_REL() {}

template<typename Inst>
ADDR A64_INST_PC_REL<Inst>::getImmPCOffsetTarget() {
    return reinterpret_cast<ADDR>(this->getImmPCOffset() + (ADDR) this->getPC());
}

//ADR ADRP

A64_ADR_ADRP::A64_ADR_ADRP() {}

A64_ADR_ADRP::A64_ADR_ADRP(STRUCT_A64(ADR_ADRP) *inst) : A64_INST_PC_REL(inst) {
    decode(inst);
}

ADDR A64_ADR_ADRP::getImmPCOffset() {
    U32 hi = get()->immhi;
    U32 lo = get()->immlo;
    ADDR offset = signExtend64(12, COMBINE(hi, lo, IMM_LO_W));
    if (isADRP()) {
        offset *= PAGE_SIZE;
    }
    return offset;
}

ADDR A64_ADR_ADRP::getImmPCOffsetTarget() {
    void * base = AlignDown(getPC(), PAGE_SIZE);
    return reinterpret_cast<ADDR>(getImmPCOffset() + (ADDR) base);
}

int A64_ADR_ADRP::getImm()  {
    return getImmPCOffset();
}

A64_ADR_ADRP::A64_ADR_ADRP(A64_ADR_ADRP::OP op, RegisterA64 *rd, int imme) : op(op), rd(rd),
                                                                             imme(imme) {
    assembler();
}

void A64_ADR_ADRP::decode(STRUCT_A64(ADR_ADRP) *decode) {

}

void A64_ADR_ADRP::assembler() {
    SET_OPCODE(ADR_ADRP);
}


//Mov Wide

A64_MOV_WIDE::A64_MOV_WIDE() {}

A64_MOV_WIDE::A64_MOV_WIDE(STRUCT_A64(MOV_WIDE) *inst) : InstructionA64(inst) {
    decode(inst);
}

A64_MOV_WIDE::A64_MOV_WIDE(A64_MOV_WIDE::OP op,RegisterA64* rd, U16 imme, U8 shift)
        : shift(shift), op(op), imme(imme), rd(rd) {
    assembler();
}

void A64_MOV_WIDE::assembler() {
    SET_OPCODE(MOV_WIDE);
    get()->imm16 = imme;
    get()->hw = static_cast<InstA64>(shift / 16);
    get()->opc = op;
    get()->sf = rd->is64Bit() ? 1 : 0;
    get()->rd = rd->getCode();
}

void A64_MOV_WIDE::decode(STRUCT_A64(MOV_WIDE) *inst) {
    imme = static_cast<U16>(inst->imm16);
    shift = static_cast<U8>(inst->hw * 16);
    op = OP(inst->opc);
    if (inst->sf == 1) {
        rd = XRegister::get(static_cast<U8>(inst->rd));
    } else {
        rd = WRegister::get(static_cast<U8>(inst->rd));
    }
}



//B BL

A64_B_BL::A64_B_BL() {}

A64_B_BL::A64_B_BL(STRUCT_A64(B_BL) *inst) : A64_INST_PC_REL(inst) {
    decode(inst);
}

A64_B_BL::A64_B_BL(A64_B_BL::OP op, ADDR offset) : op(op), offset(offset) {
    assembler();
}

ADDR A64_B_BL::getImmPCOffset() {
    return signExtend64(26 + 2, COMBINE(get()->imm26, 0b00, 2));
}

void A64_B_BL::decode(STRUCT_A64(B_BL) *inst) {
    op = OP(inst->op);
    offset = getImmPCOffset();
}

void A64_B_BL::assembler() {
    SET_OPCODE(B_BL);
    get()->op = op;
    get()->imm26 = TruncateToUint26(offset);
}



//CBZ CBNZ

A64_CBZ_CBNZ::A64_CBZ_CBNZ() {}

A64_CBZ_CBNZ::A64_CBZ_CBNZ(STRUCT_A64(CBZ_CBNZ) *inst) : A64_INST_PC_REL(inst) {
    decode(inst);
}

A64_CBZ_CBNZ::A64_CBZ_CBNZ(A64_CBZ_CBNZ::OP op, ADDR offset, RegisterA64 *rt) : op(op),
                                                                                offset(offset),
                                                                                rt(rt) {
    assembler();
}

ADDR A64_CBZ_CBNZ::getImmPCOffset() {
    return signExtend64(19 + 2, COMBINE(get()->imm19, 0b00, 2));
}

void A64_CBZ_CBNZ::decode(STRUCT_A64(CBZ_CBNZ) *inst) {
    op = OP(get()->op);
    if (inst->sf == 1) {
        rt = XRegister::get(static_cast<U8>(inst->rt));
    } else {
        rt = WRegister::get(static_cast<U8>(inst->rt));
    }
    offset = getImmPCOffset();
}

void A64_CBZ_CBNZ::assembler() {
    SET_OPCODE(CBZ_CBNZ);
    get()->op = op;
    get()->rt = rt->getCode();
    get()->sf = rt->is64Bit() ? 1 : 0;
    get()->imm19 = TruncateToUint19(offset);
}


//B.Cond

A64_B_COND::A64_B_COND() {}

A64_B_COND::A64_B_COND(STRUCT_A64(B_COND) *inst) : A64_INST_PC_REL(inst) {
    decode(inst);
}

A64_B_COND::A64_B_COND(Condition condition, ADDR offset) : condition(condition), offset(offset) {
    assembler();
}

ADDR A64_B_COND::getImmPCOffset() {
    return signExtend64(19 + 2, COMBINE(get()->imm19, 0b00, 2));
}

void A64_B_COND::decode(STRUCT_A64(B_COND) *inst) {
    condition = Condition(inst->cond);
    offset = getImmPCOffset();
}

void A64_B_COND::assembler() {
    SET_OPCODE(B_COND);
    get()->cond = condition;
    get()->imm19 = TruncateToUint19(offset);
}


//TBZ TBNZ

A64_TBZ_TBNZ::A64_TBZ_TBNZ() {}

A64_TBZ_TBNZ::A64_TBZ_TBNZ(STRUCT_A64(TBZ_TBNZ) *inst) : A64_INST_PC_REL(inst) {
    decode(inst);
}

A64_TBZ_TBNZ::A64_TBZ_TBNZ(A64_TBZ_TBNZ::OP op, RegisterA64 *rt, U32 bit, ADDR offset) : op(op),
                                                                                         rt(rt),
                                                                                         bit(bit),
                                                                                         offset(offset) {
    assembler();
}

ADDR A64_TBZ_TBNZ::getImmPCOffset() {
    return signExtend64(14 + 2, COMBINE(get()->imm14, 0b00, 2));
}

void A64_TBZ_TBNZ::decode(STRUCT_A64(TBZ_TBNZ) *inst) {
    bit = COMBINE(inst->b5, inst->b40, 5);
    if (inst->b5 == 1) {
        rt = XRegister::get(static_cast<U8>(inst->rt));
    } else {
        rt = WRegister::get(static_cast<U8>(inst->rt));
    }
    op = OP(inst->op);
    offset = getImmPCOffset();
}

void A64_TBZ_TBNZ::assembler() {
    SET_OPCODE(TBZ_TBNZ);
    get()->op = op;
    get()->b5 = rt->is64Bit() ? 1 : 0;
    get()->rt = rt->getCode();
    get()->b40 = static_cast<InstA64>(BITS(bit, sizeof(InstA64) - 5, sizeof(InstA64)));
    get()->imm14 = TruncateToUint14(offset);
}



// LDR(literal)

A64_LDR_LIT::A64_LDR_LIT() {}

A64_LDR_LIT::A64_LDR_LIT(STRUCT_A64(LDR_LIT) *inst) : A64_INST_PC_REL(inst) {
    decode(inst);
}


A64_LDR_LIT::A64_LDR_LIT(A64_LDR_LIT::OP op, RegisterA64 *rt, ADDR offset) : op(op), rt(rt),
                                                                             offset(offset) {
    assembler();
}

ADDR A64_LDR_LIT::getImmPCOffset() {
    return signExtend64(19 + 2, COMBINE(get()->imm19, 0b00, 2));
}

void A64_LDR_LIT::decode(STRUCT_A64(LDR_LIT) *inst) {
    op = OP(inst->op);
    if (op == LDR_W) {
        rt = WRegister::get(static_cast<U8>(inst->rt));
    } else {
        rt = XRegister::get(static_cast<U8>(inst->rt));
    }
    offset = getImmPCOffset();
}

void A64_LDR_LIT::assembler() {
    SET_OPCODE(LDR_LIT);
    get()->rt = rt->getCode();
    get()->op = op;
    get()->imm19 = TruncateToUint19(offset);
}



// STR IMM
A64_STR_IMM::A64_STR_IMM() {}

A64_STR_IMM::A64_STR_IMM(STRUCT_A64(STR_IMM) *inst) : InstructionA64(inst) {
    decode(inst);
}

A64_STR_IMM::A64_STR_IMM(RegisterA64 *rt, const MemOperand &operand) : rt(rt), operand(operand) {
    assembler();
}

AddrMode A64_STR_IMM::decodeAddrMode() {
    if (get()->P == 1 && get()->W == 0) {
        return Offset;
    } else if (get()->P == 0 && get()->W == 0) {
        return PostIndex;
    } else if (get()->P == 1 && get()->W == 1) {
        return PreIndex;
    } else {
        return NonAddrMode;
    }
}
