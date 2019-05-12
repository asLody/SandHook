//
// Created by swift on 2019/5/6.
//

#include "inst_arm64.h"

#define SET_OPCODE(X) get()->opcode = OPCODE_A64(X)
#define SET_OPCODE_MULTI(X, INDEX) get()->opcode##INDEX = OPCODE_A64(X##_##INDEX)

using namespace SandHook::Asm;

template<typename InstStruct>
U32 InstructionA64<InstStruct>::size() {
    return sizeof(InstA64);
}


//Unknow

A64_UNKNOW::A64_UNKNOW(STRUCT_A64(UNKNOW) &inst) : InstructionA64(&inst) {
    inst_backup = inst;
}

//PC Rel Inst

template<typename Inst>
A64_INST_PC_REL<Inst>::A64_INST_PC_REL(Inst *inst):InstructionA64<Inst>(inst) {
}

template<typename Inst>
A64_INST_PC_REL<Inst>::A64_INST_PC_REL() {}

template<typename Inst>
Addr A64_INST_PC_REL<Inst>::getImmPCOffsetTarget() {
    return this->getImmPCOffset() + reinterpret_cast<Addr>(this->getPC());
}

template<typename Inst>
bool A64_INST_PC_REL<Inst>::pcRelate() {
    return true;
}

//ADR ADRP

A64_ADR_ADRP::A64_ADR_ADRP() {}

A64_ADR_ADRP::A64_ADR_ADRP(STRUCT_A64(ADR_ADRP) &inst) : A64_INST_PC_REL(&inst) {
    decode(&inst);
}

A64_ADR_ADRP::A64_ADR_ADRP(A64_ADR_ADRP::OP op, XRegister &rd, S64 offset) : op(op), rd(&rd),
                                                                             offset(offset) {
    assembler();
}

A64_ADR_ADRP::A64_ADR_ADRP(A64_ADR_ADRP::OP op, XRegister &rd, Label &label) {
    bindLabel(label);
}

Off A64_ADR_ADRP::getImmPCOffset() {
    U32 hi = get()->immhi;
    U32 lo = get()->immlo;
    Off offset = signExtend64(IMM_LO_W + IMM_HI_W, COMBINE(hi, lo, IMM_LO_W));
    if (isADRP()) {
        offset *= P_SIZE;
    }
    return offset;
}

Addr A64_ADR_ADRP::getImmPCOffsetTarget() {
    void * base = AlignDown(getPC(), P_SIZE);
    return getImmPCOffset() + reinterpret_cast<Addr>(base);
}

void A64_ADR_ADRP::assembler() {
    SET_OPCODE(ADR_ADRP);
}

void A64_ADR_ADRP::decode(STRUCT_A64(ADR_ADRP) *inst) {
    offset = getImmPCOffsetTarget();
    rd = XReg(static_cast<U8>(get()->rd));
    op = OP(get()->op);
}

//Mov Wide

A64_MOV_WIDE::A64_MOV_WIDE() {}

A64_MOV_WIDE::A64_MOV_WIDE(STRUCT_A64(MOV_WIDE) &inst) : InstructionA64(&inst) {
    decode(&inst);
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
    get()->sf = rd->isX() ? 1 : 0;
    get()->rd = rd->getCode();
}

void A64_MOV_WIDE::decode(STRUCT_A64(MOV_WIDE) *inst) {
    imme = static_cast<U16>(inst->imm16);
    shift = static_cast<U8>(inst->hw * 16);
    op = OP(inst->opc);
    if (inst->sf == 1) {
        rd = XReg(static_cast<U8>(inst->rd));
    } else {
        rd = WReg(static_cast<U8>(inst->rd));
    }
}



//B BL

A64_B_BL::A64_B_BL() {}

A64_B_BL::A64_B_BL(STRUCT_A64(B_BL) &inst) : A64_INST_PC_REL(&inst) {
    decode(&inst);
}

A64_B_BL::A64_B_BL(A64_B_BL::OP op, Off offset) : op(op), offset(offset) {
}

A64_B_BL::A64_B_BL(A64_B_BL::OP op, Label &l) : op(op) {
    bindLabel(l);
}

Off A64_B_BL::getImmPCOffset() {
    return signExtend64(26 + 2, COMBINE(get()->imm26, 0b00, 2));
}

void A64_B_BL::onOffsetApply(Off offset) {
    this->offset = offset;
    get()->imm26 = TruncateToUint26(offset >> 2);
}

void A64_B_BL::decode(STRUCT_A64(B_BL) *inst) {
    op = OP(inst->op);
    offset = getImmPCOffset();
}

void A64_B_BL::assembler() {
    SET_OPCODE(B_BL);
    get()->op = op;
    get()->imm26 = TruncateToUint26(offset >> 2);
}



//CBZ CBNZ

A64_CBZ_CBNZ::A64_CBZ_CBNZ() {}

A64_CBZ_CBNZ::A64_CBZ_CBNZ(STRUCT_A64(CBZ_CBNZ) &inst) : A64_INST_PC_REL(&inst) {
    decode(&inst);
}

A64_CBZ_CBNZ::A64_CBZ_CBNZ(A64_CBZ_CBNZ::OP op, Off offset, RegisterA64 &rt) : op(op),
                                                                                offset(offset),
                                                                                rt(&rt) {}

A64_CBZ_CBNZ::A64_CBZ_CBNZ(A64_CBZ_CBNZ::OP op, Label& label, RegisterA64 &rt) : op(op),
                                                                               rt(&rt) {
    bindLabel(label);
}

Off A64_CBZ_CBNZ::getImmPCOffset() {
    return signExtend64(19 + 2, COMBINE(get()->imm19, 0b00, 2));
}

void A64_CBZ_CBNZ::decode(STRUCT_A64(CBZ_CBNZ) *inst) {
    op = OP(get()->op);
    if (inst->sf == 1) {
        rt = XReg(static_cast<U8>(inst->rt));
    } else {
        rt = WReg(static_cast<U8>(inst->rt));
    }
    offset = getImmPCOffset();
}

void A64_CBZ_CBNZ::assembler() {
    SET_OPCODE(CBZ_CBNZ);
    get()->op = op;
    get()->rt = rt->getCode();
    get()->sf = rt->isX() ? 1 : 0;
    get()->imm19 = TruncateToUint19(offset >> 2);
}

void A64_CBZ_CBNZ::onOffsetApply(Off offset) {
    this->offset = offset;
    get()->imm19 = TruncateToUint19(offset >> 2);
}


//B.Cond

A64_B_COND::A64_B_COND() {}

A64_B_COND::A64_B_COND(STRUCT_A64(B_COND) &inst) : A64_INST_PC_REL(&inst) {
    decode(&inst);
}

A64_B_COND::A64_B_COND(Condition condition, Off offset) : condition(condition), offset(offset) {}

Off A64_B_COND::getImmPCOffset() {
    return signExtend64(19 + 2, COMBINE(get()->imm19, 0b00, 2));
}

void A64_B_COND::decode(STRUCT_A64(B_COND) *inst) {
    condition = Condition(inst->cond);
    offset = getImmPCOffset();
}

void A64_B_COND::assembler() {
    SET_OPCODE(B_COND);
    get()->cond = condition;
    get()->imm19 = TruncateToUint19(offset >> 2);
}

A64_B_COND::A64_B_COND(Condition condition, Label &label) {
    bindLabel(label);
}

void A64_B_COND::onOffsetApply(Off offset) {
    this->offset = offset;
    get()->imm19 = TruncateToUint19(offset >> 2);
}


//TBZ TBNZ

A64_TBZ_TBNZ::A64_TBZ_TBNZ() {}

A64_TBZ_TBNZ::A64_TBZ_TBNZ(STRUCT_A64(TBZ_TBNZ) &inst) : A64_INST_PC_REL(&inst) {
    decode(&inst);
}

A64_TBZ_TBNZ::A64_TBZ_TBNZ(A64_TBZ_TBNZ::OP op, RegisterA64 &rt, U32 bit, Off offset) : op(op),
                                                                                         rt(&rt),
                                                                                         bit(bit),
                                                                                         offset(offset) {}

A64_TBZ_TBNZ::A64_TBZ_TBNZ(A64_TBZ_TBNZ::OP op, RegisterA64 &rt, U32 bit, Label &label) : op(op),
                                                                                     rt(&rt),
                                                                                     bit(bit) {
    bindLabel(label);
}

Off A64_TBZ_TBNZ::getImmPCOffset() {
    return signExtend64(14 + 2, COMBINE(get()->imm14, 0b00, 2));
}

void A64_TBZ_TBNZ::decode(STRUCT_A64(TBZ_TBNZ) *inst) {
    bit = COMBINE(inst->b5, inst->b40, 5);
    if (inst->b5 == 1) {
        rt = XReg(static_cast<U8>(inst->rt));
    } else {
        rt = WReg(static_cast<U8>(inst->rt));
    }
    op = OP(inst->op);
    offset = getImmPCOffset();
}

void A64_TBZ_TBNZ::assembler() {
    SET_OPCODE(TBZ_TBNZ);
    get()->op = op;
    get()->b5 = rt->isX() ? 1 : 0;
    get()->rt = rt->getCode();
    get()->b40 = static_cast<InstA64>(BITS(bit, sizeof(InstA64) - 5, sizeof(InstA64)));
    get()->imm14 = TruncateToUint14(offset >> 2);
}

void A64_TBZ_TBNZ::onOffsetApply(Off offset) {
    this->offset = offset;
    get()->imm14 = TruncateToUint14(offset >> 2);
}



// LDR(literal)

A64_LDR_LIT::A64_LDR_LIT() {}

A64_LDR_LIT::A64_LDR_LIT(STRUCT_A64(LDR_LIT) &inst) : A64_INST_PC_REL(&inst) {
    decode(&inst);
}


A64_LDR_LIT::A64_LDR_LIT(A64_LDR_LIT::OP op, RegisterA64 *rt, Off offset) : op(op), rt(rt),
                                                                             offset(offset) {}

Off A64_LDR_LIT::getImmPCOffset() {
    return signExtend64(19 + 2, COMBINE(get()->imm19, 0b00, 2));
}

void A64_LDR_LIT::decode(STRUCT_A64(LDR_LIT) *inst) {
    op = OP(inst->op);
    if (op == LDR_W) {
        rt = WReg(static_cast<U8>(inst->rt));
    } else {
        rt = XReg(static_cast<U8>(inst->rt));
    }
    offset = getImmPCOffset();
}

void A64_LDR_LIT::assembler() {
    SET_OPCODE(LDR_LIT);
    get()->rt = rt->getCode();
    get()->op = op;
    get()->imm19 = TruncateToUint19(offset >> 2);
}



A64_BR_BLR_RET::A64_BR_BLR_RET() {}

A64_BR_BLR_RET::A64_BR_BLR_RET(STRUCT_A64(BR_BLR_RET) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

A64_BR_BLR_RET::A64_BR_BLR_RET(A64_BR_BLR_RET::OP op, XRegister &rn) : op(op), rn(&rn) {}

void A64_BR_BLR_RET::decode(A64_STRUCT_BR_BLR_RET *inst) {
    rn = XReg(static_cast<U8>(inst->op));
    op = OP(inst->rn);
}

void A64_BR_BLR_RET::assembler() {
    SET_OPCODE_MULTI(BR_BLR_RET, 1);
    SET_OPCODE_MULTI(BR_BLR_RET, 2);
    SET_OPCODE_MULTI(BR_BLR_RET, 3);
    get()->rn = rn->getCode();
    get()->op = op;
}

A64_STR_IMM::A64_STR_IMM() {}

A64_STR_IMM::A64_STR_IMM(STRUCT_A64(STR_IMM) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

A64_STR_IMM::A64_STR_IMM(RegisterA64 &rt, const MemOperand &operand) : rt(&rt), operand(operand) {

}

void A64_STR_IMM::decode(STRUCT_A64(STR_IMM) *inst) {
    regSize = Size(inst->size);
    switch (regSize) {
        case Size64:
            rt = XReg(static_cast<U8>(inst->rt));
            operand.base = XReg(static_cast<U8>(inst->rn));
            break;
        case Size32:
            rt = WReg(static_cast<U8>(inst->rt));
            operand.base = WReg(static_cast<U8>(inst->rn));
            break;
        default:
            valid = false;
            return;
    }
    addrMode = AdMod(inst->addrmode);
    switch (addrMode) {
        case PostIndex:
            wback = true;
            postindex = true;
            operand.addr_mode = AddrMode::PostIndex;
            break;
        case PreIndex:
            wback = true;
            postindex = false;
            operand.addr_mode = AddrMode::PreIndex;
            break;
        default:
            valid = false;
            return;
    }
    scale = static_cast<U8>(inst->size);
    offset = signExtend64(9, inst->imm9);
    operand.offset = offset;
}

void A64_STR_IMM::assembler() {
    SET_OPCODE(STR_IMM);
    get()->rt = rt->getCode();
    get()->rn = operand.base->getCode();
    get()->imm9 = TruncateToUint9(operand.offset);
    if (rt->isX()) {
        get()->size = Size64;
    } else if (rt->isW()) {
        get()->size = Size32;
    } else {
        valid = false;
        return;
    }
    switch (operand.addr_mode) {
        case AddrMode::PostIndex:
            wback = true;
            postindex = true;
            get()->addrmode = PostIndex;
            break;
        case AddrMode::PreIndex:
            wback = true;
            postindex = false;
            get()->addrmode = PreIndex;
            break;
        default:
            valid = false;
            return;
    }
}

A64_STR_UIMM::A64_STR_UIMM() {}

A64_STR_UIMM::A64_STR_UIMM(STRUCT_A64(STR_UIMM) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

A64_STR_UIMM::A64_STR_UIMM(RegisterA64 &rt, const MemOperand &operand) : rt(&rt), operand(operand) {

}

void A64_STR_UIMM::decode(STRUCT_A64(STR_UIMM) *inst) {
    regSize = Size(inst->size);
    switch (regSize) {
        case Size64:
            rt = XReg(static_cast<U8>(inst->rt));
            operand.base = XReg(static_cast<U8>(inst->rn));
            break;
        case Size32:
            rt = WReg(static_cast<U8>(inst->rt));
            operand.base = WReg(static_cast<U8>(inst->rn));
            break;
        default:
            valid = false;
            return;
    }
    operand.addr_mode = AddrMode::Offset;
    scale = static_cast<U8>(inst->size);
    offset = inst->imm12 << regSize;
    operand.offset = offset;
}

void A64_STR_UIMM::assembler() {
    SET_OPCODE(STR_IMM);
    get()->rt = rt->getCode();
    get()->rn = operand.base->getCode();
    if (rt->isX()) {
        get()->size = Size64;
    } else if (rt->isW()) {
        get()->size = Size32;
    } else {
        valid = false;
        return;
    }
    get()->imm12 = operand.offset >> get()->size;
}



A64_MOV_REG::A64_MOV_REG() {}

A64_MOV_REG::A64_MOV_REG(STRUCT_A64(MOV_REG) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

A64_MOV_REG::A64_MOV_REG(RegisterA64 &rd, RegisterA64 &rm) : rd(&rd), rm(&rm) {
}

void A64_MOV_REG::decode(A64_STRUCT_MOV_REG *inst) {
    if (inst->sf == 1) {
        rd = XReg(static_cast<U8>(inst->rd));
        rm = XReg(static_cast<U8>(inst->rm));
    } else {
        rd = WReg(static_cast<U8>(inst->rd));
        rm = WReg(static_cast<U8>(inst->rm));
    }
}

void A64_MOV_REG::assembler() {
    SET_OPCODE_MULTI(MOV_REG, 1);
    SET_OPCODE_MULTI(MOV_REG, 2);
    get()->sf = rd->isX() ? 1 : 0;
    get()->rd = rd->getCode();
    get()->rm = rm->getCode();
}



A64_SUB_EXT_REG::A64_SUB_EXT_REG() {}

A64_SUB_EXT_REG::A64_SUB_EXT_REG(STRUCT_A64(SUB_EXT_REG) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

A64_SUB_EXT_REG::A64_SUB_EXT_REG(S s, RegisterA64 &rd, RegisterA64 &rn, const Operand &operand,
                                   FlagsUpdate flagsUpdate) : s(s), rd(&rd), rn(&rn), operand(operand),
                                                              flagsUpdate(flagsUpdate) {}

void A64_SUB_EXT_REG::decode(STRUCT_A64(SUB_EXT_REG) *inst) {
    s = S(inst->S);
    if (inst->sf == 1) {
        rd = XReg(static_cast<U8>(inst->rd));
        rn = XReg(static_cast<U8>(inst->rn));
        operand.reg = XReg(static_cast<U8>(inst->rm));
    } else {
        rd = WReg(static_cast<U8>(inst->rd));
        rn = WReg(static_cast<U8>(inst->rn));
        operand.reg = XReg(static_cast<U8>(inst->rm));
    }
    operand.extend = Extend(inst->option);
    INST_ASSERT(inst->imm3 > 4);
    operand.shift = Shift(inst->imm3);
}

void A64_SUB_EXT_REG::assembler() {
    SET_OPCODE_MULTI(SUB_EXT_REG, 1);
    SET_OPCODE_MULTI(SUB_EXT_REG, 2);
    get()->S = s;
    get()->sf = rd->isX() ? 1 : 0;
    get()->option = operand.extend;
    get()->imm3 = operand.shift;
    get()->rm = operand.reg->getCode();
    get()->rn = rn->getCode();
    get()->rd = rd->getCode();
}



A64_EXCEPTION_GEN::A64_EXCEPTION_GEN() {}

A64_EXCEPTION_GEN::A64_EXCEPTION_GEN(STRUCT_A64(EXCEPTION_GEN) &inst) : InstructionA64(&inst) {
    decode(&inst);
}


A64_EXCEPTION_GEN::A64_EXCEPTION_GEN(A64_EXCEPTION_GEN::OP op, ExceptionLevel el, U16 imme) : op(
        op), el(el), imme(imme) {}

void A64_EXCEPTION_GEN::decode(STRUCT_A64(EXCEPTION_GEN) *inst) {
    op = OP(inst->op);
    el = ExceptionLevel(inst->ll);
    imme = static_cast<U16>(inst->imm16);
}

void A64_EXCEPTION_GEN::assembler() {
    SET_OPCODE_MULTI(EXCEPTION_GEN, 1);
    SET_OPCODE_MULTI(EXCEPTION_GEN, 2);
    get()->op = op;
    get()->ll = el;
    get()->imm16 = imme;
}


A64_SVC::A64_SVC(U16 imme) : A64_EXCEPTION_GEN(XXC, EL1,imme) {}

A64_SVC::A64_SVC() {}

A64_SVC::A64_SVC(STRUCT_A64(SVC) &inst) : A64_EXCEPTION_GEN(inst) {}
