//
// Created by swift on 2019/5/6.
//

#include "inst_arm64.h"

#define SET_OPCODE(X) get()->opcode = OPCODE_A64(X)
#define SET_OPCODE_MULTI(X, INDEX) get()->opcode##INDEX = OPCODE_A64(X##_##INDEX)

#define DECODE_OFFSET(bits, ext) signExtend64(bits + ext, COMBINE(get()->imm##bits, 0, ext))
#define ENCODE_OFFSET(bits, ext) get()->imm##bits = TruncateToUint##bits(offset >> ext)

using namespace SandHook::Asm;
using namespace SandHook::AsmA64;

template<typename InstStruct>
U32 InstructionA64<InstStruct>::size() {
    return sizeof(InstA64);
}


//Unknow

A64_UNKNOW::A64_UNKNOW(STRUCT_A64(UNKNOW) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

void A64_UNKNOW::decode(A64_STRUCT_UNKNOW *inst) {
    inst_backup = *inst;
}

void A64_UNKNOW::assembler() {
    set(inst_backup);
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
}

A64_ADR_ADRP::A64_ADR_ADRP(A64_ADR_ADRP::OP op, XRegister &rd, Label &label) {
    bindLabel(label);
}

Off A64_ADR_ADRP::getImmPCOffset() {
    U32 hi = get()->immhi;
    U32 lo = get()->immlo;
    U64 imm = COMBINE(hi, lo, IMM_LO_W);
    if (isADRP()) {
        return signExtend64(IMM_HI_W + IMM_LO_W + PAGE_OFFSET, imm << PAGE_OFFSET);
    } else {
        return signExtend64(IMM_HI_W + IMM_LO_W, imm);
    }
}

Addr A64_ADR_ADRP::getImmPCOffsetTarget() {
    void* base = AlignDown(getPC(), P_SIZE);
    return offset + reinterpret_cast<Addr>(base);
}

void A64_ADR_ADRP::assembler() {
    SET_OPCODE(ADR_ADRP);
}

void A64_ADR_ADRP::decode(STRUCT_A64(ADR_ADRP) *inst) {
    offset = getImmPCOffset();
    DECODE_RD(XReg);
    DECODE_OP;
}

//Mov Wide

A64_MOV_WIDE::A64_MOV_WIDE() {}

A64_MOV_WIDE::A64_MOV_WIDE(STRUCT_A64(MOV_WIDE) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

A64_MOV_WIDE::A64_MOV_WIDE(A64_MOV_WIDE::OP op,RegisterA64* rd, U16 imme, U8 shift)
        : shift(shift), op(op), imme(imme), rd(rd) {}

void A64_MOV_WIDE::assembler() {
    SET_OPCODE(MOV_WIDE);
    get()->imm16 = imme;
    get()->hw = static_cast<InstA64>(shift / 16);
    ENCODE_OP;
    get()->sf = rd->isX() ? 1 : 0;
    ENCODE_RD;
}

void A64_MOV_WIDE::decode(STRUCT_A64(MOV_WIDE) *inst) {
    imme = static_cast<U16>(inst->imm16);
    shift = static_cast<U8>(inst->hw * 16);
    op = OP(inst->op);
    if (inst->sf == 1) {
        DECODE_RD(XReg);
    } else {
        DECODE_RD(WReg);
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
    return DECODE_OFFSET(26, 2);
}

void A64_B_BL::onOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(26, 2);
}

void A64_B_BL::decode(STRUCT_A64(B_BL) *inst) {
    DECODE_OP;
    offset = getImmPCOffset();
}

void A64_B_BL::assembler() {
    SET_OPCODE(B_BL);
    ENCODE_OP;
    ENCODE_OFFSET(26, 2);
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
    return DECODE_OFFSET(19, 2);
}

void A64_CBZ_CBNZ::decode(STRUCT_A64(CBZ_CBNZ) *inst) {
    DECODE_OP;
    if (inst->sf == 1) {
        DECODE_RT(XReg);
    } else {
        DECODE_RT(WReg);
    }
    offset = getImmPCOffset();
}

void A64_CBZ_CBNZ::assembler() {
    SET_OPCODE(CBZ_CBNZ);
    ENCODE_OP;
    ENCODE_RT;
    get()->sf = rt->isX() ? 1 : 0;
    ENCODE_OFFSET(19, 2);
}

void A64_CBZ_CBNZ::onOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(19, 2);
}


//B.Cond

A64_B_COND::A64_B_COND() {}

A64_B_COND::A64_B_COND(STRUCT_A64(B_COND) &inst) : A64_INST_PC_REL(&inst) {
    decode(&inst);
}

A64_B_COND::A64_B_COND(Condition condition, Off offset) : condition(condition), offset(offset) {}

Off A64_B_COND::getImmPCOffset() {
    return DECODE_OFFSET(19, 2);
}

void A64_B_COND::decode(STRUCT_A64(B_COND) *inst) {
    DECODE_COND;
    offset = getImmPCOffset();
}

void A64_B_COND::assembler() {
    SET_OPCODE(B_COND);
    ENCODE_COND;
    ENCODE_OFFSET(19, 2);
}

A64_B_COND::A64_B_COND(Condition condition, Label &label) {
    bindLabel(label);
}

void A64_B_COND::onOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(19, 2);
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
    return DECODE_OFFSET(14, 2);
}

void A64_TBZ_TBNZ::decode(STRUCT_A64(TBZ_TBNZ) *inst) {
    bit = COMBINE(inst->b5, inst->b40, 5);
    if (inst->b5 == 1) {
        DECODE_RT(XReg);
    } else {
        DECODE_RT(WReg);
    }
    DECODE_OP;
    offset = getImmPCOffset();
}

void A64_TBZ_TBNZ::assembler() {
    SET_OPCODE(TBZ_TBNZ);
    ENCODE_OP;
    get()->b5 = rt->isX() ? 1 : 0;
    ENCODE_RT;
    get()->b40 = static_cast<InstA64>(BITS(bit, sizeof(InstA64) - 5, sizeof(InstA64)));
    ENCODE_OFFSET(14, 2);
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

A64_LDR_LIT::A64_LDR_LIT(A64_LDR_LIT::OP op, RegisterA64 &rt, Off offset) : op(op), rt(&rt),
                                                                             offset(offset) {}

A64_LDR_LIT::A64_LDR_LIT(A64_LDR_LIT::OP op, RegisterA64 &rt, Label& label) : op(op), rt(&rt) {
    bindLabel(label);
}

Off A64_LDR_LIT::getImmPCOffset() {
    return DECODE_OFFSET(19, 2);
}

void A64_LDR_LIT::onOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(19, 2);
}

void A64_LDR_LIT::decode(STRUCT_A64(LDR_LIT) *inst) {
    DECODE_OP;
    offset = getImmPCOffset();
    ENCODE_OFFSET(19, 2);
    if (op == LDR_W) {
        DECODE_RT(WReg);
    } else {
        DECODE_RT(XReg);
    }
}

void A64_LDR_LIT::assembler() {
    SET_OPCODE(LDR_LIT);
    ENCODE_OP;
    ENCODE_RT;
    ENCODE_OFFSET(19, 2);
}



A64_BR_BLR_RET::A64_BR_BLR_RET() {}

A64_BR_BLR_RET::A64_BR_BLR_RET(STRUCT_A64(BR_BLR_RET) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

A64_BR_BLR_RET::A64_BR_BLR_RET(A64_BR_BLR_RET::OP op, XRegister &rn) : op(op), rn(&rn) {}

void A64_BR_BLR_RET::decode(A64_STRUCT_BR_BLR_RET *inst) {
    DECODE_RN(XReg);
    DECODE_OP;
}

void A64_BR_BLR_RET::assembler() {
    SET_OPCODE_MULTI(BR_BLR_RET, 1);
    SET_OPCODE_MULTI(BR_BLR_RET, 2);
    SET_OPCODE_MULTI(BR_BLR_RET, 3);
    ENCODE_RN;
    ENCODE_OP;
}

A64_STR_IMM::A64_STR_IMM() {}

A64_STR_IMM::A64_STR_IMM(STRUCT_A64(STR_IMM) &inst) : A64LoadAndStoreImm(&inst) {
    decode(&inst);
}

A64_STR_IMM::A64_STR_IMM(RegisterA64 &rt, const MemOperand &operand) : A64LoadAndStoreImm(&rt, operand) {

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
            operand.addr_mode = AddrMode::Offset;
            valid = false;
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
            get()->addrmode = Offset;
            valid = false;
    }
}

A64_STR_UIMM::A64_STR_UIMM() {}

A64_STR_UIMM::A64_STR_UIMM(STRUCT_A64(STR_UIMM) &inst) : A64LoadAndStoreImm(&inst) {
    decode(&inst);
}

A64_STR_UIMM::A64_STR_UIMM(RegisterA64 &rt, const MemOperand &operand) :  A64LoadAndStoreImm(&rt, operand) {

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
    ENCODE_RT;
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
        DECODE_RD(XReg);
        DECODE_RM(XReg);
    } else {
        DECODE_RD(WReg);
        DECODE_RM(WReg);
    }
}

void A64_MOV_REG::assembler() {
    SET_OPCODE_MULTI(MOV_REG, 1);
    SET_OPCODE_MULTI(MOV_REG, 2);
    get()->sf = rd->isX() ? 1 : 0;
    ENCODE_RD;
    ENCODE_RM;
}



A64_SUB_EXT_REG::A64_SUB_EXT_REG() {}

A64_SUB_EXT_REG::A64_SUB_EXT_REG(STRUCT_A64(SUB_EXT_REG) &inst) : InstructionA64(&inst) {
    decode(&inst);
}

A64_SUB_EXT_REG::A64_SUB_EXT_REG(RegisterA64 &rd, RegisterA64 &rn, const Operand &operand,
                                   FlagsUpdate flagsUpdate) : rd(&rd), rn(&rn), operand(operand),
                                                              flagsUpdate(flagsUpdate) {}

void A64_SUB_EXT_REG::decode(STRUCT_A64(SUB_EXT_REG) *inst) {
    flagsUpdate = FlagsUpdate(inst->S);
    if (inst->sf == 1) {
        DECODE_RD(XReg);
        DECODE_RN(XReg);
        operand.reg = XReg(static_cast<U8>(inst->rm));
    } else {
        DECODE_RD(WReg);
        DECODE_RN(WReg);
        operand.reg = XReg(static_cast<U8>(inst->rm));
    }
    operand.extend = Extend(inst->option);
    INST_ASSERT(inst->imm3 > 4);
    operand.shift = Shift(inst->imm3);
}

void A64_SUB_EXT_REG::assembler() {
    SET_OPCODE_MULTI(SUB_EXT_REG, 1);
    SET_OPCODE_MULTI(SUB_EXT_REG, 2);
    get()->S = flagsUpdate;
    get()->sf = rd->isX() ? 1 : 0;
    get()->option = operand.extend;
    get()->imm3 = operand.shift;
    get()->rm = operand.reg->getCode();
    ENCODE_RN;
    ENCODE_RD;
}



A64_EXCEPTION_GEN::A64_EXCEPTION_GEN() {}

A64_EXCEPTION_GEN::A64_EXCEPTION_GEN(STRUCT_A64(EXCEPTION_GEN) &inst) : InstructionA64(&inst) {
    decode(&inst);
}


A64_EXCEPTION_GEN::A64_EXCEPTION_GEN(A64_EXCEPTION_GEN::OP op, ExceptionLevel el, U16 imme) : op(
        op), el(el), imme(imme) {}

void A64_EXCEPTION_GEN::decode(STRUCT_A64(EXCEPTION_GEN) *inst) {
    DECODE_OP;
    el = ExceptionLevel(inst->ll);
    imme = static_cast<U16>(inst->imm16);
}

void A64_EXCEPTION_GEN::assembler() {
    SET_OPCODE_MULTI(EXCEPTION_GEN, 1);
    SET_OPCODE_MULTI(EXCEPTION_GEN, 2);
    ENCODE_OP;
    get()->ll = el;
    get()->imm16 = imme;
}


A64_SVC::A64_SVC(U16 imme) : A64_EXCEPTION_GEN(XXC, EL1,imme) {}

A64_SVC::A64_SVC() {}

A64_SVC::A64_SVC(STRUCT_A64(SVC) &inst) : A64_EXCEPTION_GEN(inst) {}



A64_LDR_IMM::A64_LDR_IMM() {}

A64_LDR_IMM::A64_LDR_IMM(STRUCT_A64(LDR_IMM) &inst) : A64LoadAndStoreImm(&inst) {
    decode(&inst);
}

A64_LDR_IMM::A64_LDR_IMM(RegisterA64 &rt, const MemOperand &operand) : A64LoadAndStoreImm(&rt,
                                                                                          operand) {}

void A64_LDR_IMM::decode(STRUCT_A64(LDR_IMM) *inst) {
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
            operand.addr_mode = AddrMode::Offset;
            valid = false;
    }
    scale = static_cast<U8>(inst->size);
    offset = signExtend64(9, inst->imm9);
    operand.offset = offset;
}

void A64_LDR_IMM::assembler() {
    SET_OPCODE(LDR_IMM);
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
            get()->addrmode = Offset;
            valid = false;
    }
}



A64_LDR_UIMM::A64_LDR_UIMM() {}

A64_LDR_UIMM::A64_LDR_UIMM(STRUCT_A64(LDR_UIMM) &inst) : A64LoadAndStoreImm(&inst) {
    decode(&inst);
}

A64_LDR_UIMM::A64_LDR_UIMM(RegisterA64 &rt, const MemOperand &operand) : A64LoadAndStoreImm(&rt,
                                                                                          operand) {}

void A64_LDR_UIMM::decode(STRUCT_A64(LDR_UIMM) *inst) {
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

void A64_LDR_UIMM::assembler() {
    SET_OPCODE(LDR_UIMM);
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
    get()->imm12 = static_cast<InstA64>(operand.offset >> get()->size);
}


A64_LDRSW_IMM::A64_LDRSW_IMM() {}

A64_LDRSW_IMM::A64_LDRSW_IMM(STRUCT_A64(LDRSW_IMM) &inst) : A64_LDR_IMM(inst) {}

A64_LDRSW_IMM::A64_LDRSW_IMM(RegisterA64 &rt, const MemOperand &operand) : A64_LDR_IMM(rt,
                                                                                       operand) {}

void A64_LDRSW_IMM::decode(STRUCT_A64(LDR_IMM) *inst) {
    rt = XReg(static_cast<U8>(inst->rt));
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
    rt = XReg(static_cast<U8>(inst->rt));
    operand.base = XReg(static_cast<U8>(inst->rn));
}

void A64_LDRSW_IMM::assembler() {
    SET_OPCODE(LDRSW_IMM);
    get()->size = Size32;
    get()->rt = rt->getCode();
    get()->rn = operand.base->getCode();
    get()->imm9 = TruncateToUint9(operand.offset);
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




A64_LDRSW_UIMM::A64_LDRSW_UIMM() {}

A64_LDRSW_UIMM::A64_LDRSW_UIMM(STRUCT_A64(LDR_UIMM) &inst) : A64_LDR_UIMM(inst) {}

A64_LDRSW_UIMM::A64_LDRSW_UIMM(XRegister &rt, const MemOperand &operand) : A64_LDR_UIMM(rt,
                                                                                          operand) {}

void A64_LDRSW_UIMM::decode(STRUCT_A64(LDR_UIMM) *inst) {
    DECODE_RT(XReg);
    operand.base = XReg(static_cast<U8>(inst->rn));
    operand.addr_mode = AddrMode::Offset;
    scale = static_cast<U8>(inst->size);
    offset = inst->imm12 << Size32;
    operand.offset = offset;
}

void A64_LDRSW_UIMM::assembler() {
    SET_OPCODE(LDRSW_UIMM);
    get()->size = Size32;
    ENCODE_RT;
    get()->rn = operand.base->getCode();
    get()->imm12 = operand.offset >> Size32;
}
