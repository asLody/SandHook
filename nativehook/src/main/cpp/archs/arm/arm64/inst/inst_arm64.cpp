//
// Created by swift on 2019/5/6.
//

#include "inst_arm64.h"

#define SET_OPCODE(X) Get()->opcode = OPCODE_A64(X)
#define SET_OPCODE_MULTI(X, INDEX) Get()->opcode##INDEX = OPCODE_A64(X##_##INDEX)

#define DECODE_OFFSET(bits, ext) SignExtend64(bits + ext, COMBINE(Get()->imm##bits, 0, ext))
#define ENCODE_OFFSET(bits, ext) Get()->imm##bits = TruncateToUint##bits(offset >> ext)

using namespace SandHook::Asm;
using namespace SandHook::AsmA64;

//PC Rel Inst

template<typename S,U32 C>
Addr A64_INST_PC_REL<S,C>::GetImmPCOffsetTarget() {
    return this->GetImmPCOffset() + reinterpret_cast<Addr>(this->GetPC());
}

template<typename S,U32 C>
bool A64_INST_PC_REL<S,C>::PcRelate() {
    return true;
}

template<typename S, U32 C>
A64_INST_PC_REL<S, C>::A64_INST_PC_REL() {}

template<typename S, U32 C>
A64_INST_PC_REL<S, C>::A64_INST_PC_REL(void *inst) : InstructionA64<S,C>(inst) {

}

//ADR ADRP


A64_ADR_ADRP::A64_ADR_ADRP(void *inst) : A64_INST_PC_REL(inst) {}

A64_ADR_ADRP::A64_ADR_ADRP(A64_ADR_ADRP::OP op, XRegister &rd, S64 offset) : op(op), rd(&rd),
                                                                             offset(offset) {
}

A64_ADR_ADRP::A64_ADR_ADRP(A64_ADR_ADRP::OP op, XRegister &rd, Label *label) {
    BindLabel(label);
}

Off A64_ADR_ADRP::GetImmPCOffset() {
    U32 hi = Get()->immhi;
    U32 lo = Get()->immlo;
    U64 imm = COMBINE(hi, lo, IMM_LO_W);
    if (IsADRP()) {
        return SignExtend64(IMM_HI_W + IMM_LO_W + PAGE_OFFSET, imm << PAGE_OFFSET);
    } else {
        return SignExtend64(IMM_HI_W + IMM_LO_W, imm);
    }
}

Addr A64_ADR_ADRP::GetImmPCOffsetTarget() {
    Addr base = AlignDown((Addr) GetPC(), PAGE_SIZE);
    return offset + base;
}

void A64_ADR_ADRP::Assemble() {
    SET_OPCODE(ADR_ADRP);
}

void A64_ADR_ADRP::Disassemble() {
    offset = GetImmPCOffset();
    DECODE_RD(XReg);
    DECODE_OP;
}

//Mov Wide

A64_MOV_WIDE::A64_MOV_WIDE(void* inst) : InstructionA64(inst) {}

A64_MOV_WIDE::A64_MOV_WIDE(A64_MOV_WIDE::OP op,RegisterA64* rd, U16 imme, U8 shift)
        : shift(shift), op(op), imme(imme), rd(rd) {}

void A64_MOV_WIDE::Assemble() {
    SET_OPCODE(MOV_WIDE);
    Get()->imm16 = imme;
    Get()->hw = static_cast<InstA64>(shift / 16);
    ENCODE_OP;
    Get()->sf = rd->isX() ? 1 : 0;
    ENCODE_RD;
}

void A64_MOV_WIDE::Disassemble() {
    imme = static_cast<U16>(Get()->imm16);
    shift = static_cast<U8>(Get()->hw * 16);
    op = OP(Get()->op);
    if (Get()->sf == 1) {
        DECODE_RD(XReg);
    } else {
        DECODE_RD(WReg);
    }
}



//B BL

A64_B_BL::A64_B_BL(void *inst) : A64_INST_PC_REL(inst) {
}

A64_B_BL::A64_B_BL(A64_B_BL::OP op, Off offset) : op(op), offset(offset) {
}

A64_B_BL::A64_B_BL(A64_B_BL::OP op, Label *l) : op(op) {
    BindLabel(l);
}

Off A64_B_BL::GetImmPCOffset() {
    return DECODE_OFFSET(26, 2);
}

void A64_B_BL::OnOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(26, 2);
}

void A64_B_BL::Disassemble() {
    DECODE_OP;
    offset = GetImmPCOffset();
}

void A64_B_BL::Assemble() {
    SET_OPCODE(B_BL);
    ENCODE_OP;
    ENCODE_OFFSET(26, 2);
}



//CBZ CBNZ


A64_CBZ_CBNZ::A64_CBZ_CBNZ(void *inst) : A64_INST_PC_REL(inst) {}

A64_CBZ_CBNZ::A64_CBZ_CBNZ(A64_CBZ_CBNZ::OP op, Off offset, RegisterA64 &rt) : op(op),
                                                                               offset(offset),
                                                                               rt(&rt) {}

A64_CBZ_CBNZ::A64_CBZ_CBNZ(A64_CBZ_CBNZ::OP op, Label *label, RegisterA64 &rt) : op(op),
                                                                                 rt(&rt) {
    BindLabel(label);
}

Off A64_CBZ_CBNZ::GetImmPCOffset() {
    return DECODE_OFFSET(19, 2);
}

void A64_CBZ_CBNZ::Disassemble() {
    DECODE_OP;
    if (Get()->sf == 1) {
        DECODE_RT(XReg);
    } else {
        DECODE_RT(WReg);
    }
    offset = GetImmPCOffset();
}

void A64_CBZ_CBNZ::Assemble() {
    SET_OPCODE(CBZ_CBNZ);
    ENCODE_OP;
    ENCODE_RT;
    Get()->sf = rt->isX() ? 1 : 0;
    ENCODE_OFFSET(19, 2);
}

void A64_CBZ_CBNZ::OnOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(19, 2);
}


//B.Cond

A64_B_COND::A64_B_COND(void *inst) : A64_INST_PC_REL(inst) {}

A64_B_COND::A64_B_COND(Condition condition, Off offset) : condition(condition), offset(offset) {}


A64_B_COND::A64_B_COND(Condition condition, Label *label) {
    BindLabel(label);
}


Off A64_B_COND::GetImmPCOffset() {
    return DECODE_OFFSET(19, 2);
}

void A64_B_COND::Disassemble() {
    DECODE_COND;
    offset = GetImmPCOffset();
}

void A64_B_COND::Assemble() {
    SET_OPCODE(B_COND);
    ENCODE_COND;
    ENCODE_OFFSET(19, 2);
}

void A64_B_COND::OnOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(19, 2);
}


//TBZ TBNZ


A64_TBZ_TBNZ::A64_TBZ_TBNZ(void *inst) : A64_INST_PC_REL(inst) {}

A64_TBZ_TBNZ::A64_TBZ_TBNZ(A64_TBZ_TBNZ::OP op, RegisterA64 &rt, U32 bit, Off offset) : op(op),
                                                                                        rt(&rt),
                                                                                        bit(bit),
                                                                                        offset(offset) {}

A64_TBZ_TBNZ::A64_TBZ_TBNZ(A64_TBZ_TBNZ::OP op, RegisterA64 &rt, U32 bit, Label *label) : op(op),
                                                                                          rt(&rt),
                                                                                          bit(bit) {
    BindLabel(label);
}

Off A64_TBZ_TBNZ::GetImmPCOffset() {
    return DECODE_OFFSET(14, 2);
}

void A64_TBZ_TBNZ::Disassemble() {
    bit = COMBINE(Get()->b5, Get()->b40, 5);
    if (Get()->b5 == 1) {
        DECODE_RT(XReg);
    } else {
        DECODE_RT(WReg);
    }
    DECODE_OP;
    offset = GetImmPCOffset();
}

void A64_TBZ_TBNZ::Assemble() {
    SET_OPCODE(TBZ_TBNZ);
    ENCODE_OP;
    Get()->b5 = rt->isX() ? 1 : 0;
    ENCODE_RT;
    Get()->b40 = static_cast<InstA64>(BITS(bit, sizeof(InstA64) - 5, sizeof(InstA64)));
    ENCODE_OFFSET(14, 2);
}

void A64_TBZ_TBNZ::OnOffsetApply(Off offset) {
    this->offset = offset;
    Get()->imm14 = TruncateToUint14(offset >> 2);
}



// LDR(literal)

A64_LDR_LIT::A64_LDR_LIT(void *inst) : A64_INST_PC_REL(inst) {}

A64_LDR_LIT::A64_LDR_LIT(A64_LDR_LIT::OP op, RegisterA64 &rt, Off offset) : op(op), rt(&rt),
                                                                            offset(offset) {}

A64_LDR_LIT::A64_LDR_LIT(A64_LDR_LIT::OP op, RegisterA64 &rt, Label *label) : op(op), rt(&rt) {
    BindLabel(label);
}

Off A64_LDR_LIT::GetImmPCOffset() {
    return DECODE_OFFSET(19, 2);
}

void A64_LDR_LIT::OnOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(19, 2);
}

void A64_LDR_LIT::Disassemble() {
    DECODE_OP;
    offset = GetImmPCOffset();
    if (op == LDR_W) {
        DECODE_RT(WReg);
    } else {
        DECODE_RT(XReg);
    }
}

void A64_LDR_LIT::Assemble() {
    SET_OPCODE(LDR_LIT);
    ENCODE_OP;
    ENCODE_RT;
    ENCODE_OFFSET(19, 2);
}


//BR BLR RET

A64_BR_BLR_RET::A64_BR_BLR_RET(void *inst) : InstructionA64(inst) {}

A64_BR_BLR_RET::A64_BR_BLR_RET(A64_BR_BLR_RET::OP op, XRegister &rn) : op(op), rn(&rn) {}

void A64_BR_BLR_RET::Disassemble() {
    DECODE_RN(XReg);
    DECODE_OP;
}

void A64_BR_BLR_RET::Assemble() {
    SET_OPCODE_MULTI(BR_BLR_RET, 1);
    SET_OPCODE_MULTI(BR_BLR_RET, 2);
    SET_OPCODE_MULTI(BR_BLR_RET, 3);
    ENCODE_RN;
    ENCODE_OP;
}


//STR IMM

A64_STR_IMM::A64_STR_IMM(void *inst) : A64LoadAndStoreImm(inst) {}

A64_STR_IMM::A64_STR_IMM(RegisterA64 &rt, const MemOperand &operand) : A64LoadAndStoreImm(&rt, operand) {
}

void A64_STR_IMM::Disassemble() {
    regSize = RegSize(Get()->size);
    switch (regSize) {
        case Size64:
            rt = XReg(static_cast<U8>(Get()->rt));
            operand.base_ = XReg(static_cast<U8>(Get()->rn));
            break;
        case Size32:
            rt = WReg(static_cast<U8>(Get()->rt));
            operand.base_ = WReg(static_cast<U8>(Get()->rn));
            break;
        default:
            valid_ = false;
            return;
    }
    addrMode = AdMod(Get()->addrmode);
    switch (addrMode) {
        case PostIndex:
            wback = true;
            postindex = true;
            operand.addr_mode_ = AddrMode::PostIndex;
            break;
        case PreIndex:
            wback = true;
            postindex = false;
            operand.addr_mode_ = AddrMode::PreIndex;
            break;
        default:
            operand.addr_mode_ = AddrMode::Offset;
            valid_ = false;
    }
    scale = static_cast<U8>(Get()->size);
    offset = SignExtend64(9, Get()->imm9);
    operand.offset_ = offset;
}

void A64_STR_IMM::Assemble() {
    SET_OPCODE(STR_IMM);
    Get()->rt = rt->Code();
    Get()->rn = operand.base_->Code();
    Get()->imm9 = TruncateToUint9(operand.offset_);
    if (rt->isX()) {
        Get()->size = Size64;
    } else if (rt->isW()) {
        Get()->size = Size32;
    } else {
        valid_ = false;
        return;
    }
    switch (operand.addr_mode_) {
        case AddrMode::PostIndex:
            wback = true;
            postindex = true;
            Get()->addrmode = PostIndex;
            break;
        case AddrMode::PreIndex:
            wback = true;
            postindex = false;
            Get()->addrmode = PreIndex;
            break;
        default:
            Get()->addrmode = Offset;
            valid_ = false;
    }
}



A64_STR_UIMM::A64_STR_UIMM(void *inst) : A64LoadAndStoreImm(inst) {}

A64_STR_UIMM::A64_STR_UIMM(RegisterA64 &rt, const MemOperand &operand) :  A64LoadAndStoreImm(&rt, operand) {
}

void A64_STR_UIMM::Disassemble() {
    regSize = RegSize(Get()->size);
    switch (regSize) {
        case Size64:
            rt = XReg(static_cast<U8>(Get()->rt));
            operand.base_ = XReg(static_cast<U8>(Get()->rn));
            break;
        case Size32:
            rt = WReg(static_cast<U8>(Get()->rt));
            operand.base_ = WReg(static_cast<U8>(Get()->rn));
            break;
        default:
            valid_ = false;
            return;
    }
    operand.addr_mode_ = AddrMode::Offset;
    scale = static_cast<U8>(Get()->size);
    offset = Get()->imm12 << regSize;
    operand.offset_ = offset;
}

void A64_STR_UIMM::Assemble() {
    SET_OPCODE(STR_UIMM);
    ENCODE_RT;
    Get()->rn = operand.base_->Code();
    if (rt->isX()) {
        Get()->size = Size64;
    } else if (rt->isW()) {
        Get()->size = Size32;
    } else {
        valid_ = false;
        return;
    }
    Get()->imm12 = operand.offset_ >> Get()->size;
}


A64_MOV_REG::A64_MOV_REG(void *inst) : InstructionA64(inst) {
}

A64_MOV_REG::A64_MOV_REG(RegisterA64 &rd, RegisterA64 &rm) : rd(&rd), rm(&rm) {
}

void A64_MOV_REG::Disassemble() {
    if (Get()->sf == 1) {
        DECODE_RD(XReg);
        DECODE_RM(XReg);
    } else {
        DECODE_RD(WReg);
        DECODE_RM(WReg);
    }
}

void A64_MOV_REG::Assemble() {
    SET_OPCODE_MULTI(MOV_REG, 1);
    SET_OPCODE_MULTI(MOV_REG, 2);
    Get()->sf = rd->isX() ? 1 : 0;
    ENCODE_RD;
    ENCODE_RM;
}

A64_SUB_EXT_REG::A64_SUB_EXT_REG(void *inst) : InstructionA64(inst) {}

A64_SUB_EXT_REG::A64_SUB_EXT_REG(RegisterA64 &rd, RegisterA64 &rn, const Operand &operand,
                                 FlagsUpdate flagsUpdate) : rd(&rd), rn(&rn), operand(operand),
                                                            flagsUpdate(flagsUpdate) {}

void A64_SUB_EXT_REG::Disassemble() {
    flagsUpdate = FlagsUpdate(Get()->S);
    if (Get()->sf == 1) {
        DECODE_RD(XReg);
        DECODE_RN(XReg);
        operand.reg_ = XReg(static_cast<U8>(Get()->rm));
    } else {
        DECODE_RD(WReg);
        DECODE_RN(WReg);
        operand.reg_ = XReg(static_cast<U8>(Get()->rm));
    }
    operand.extend_ = Extend(Get()->option);
    INST_ASSERT(Get()->imm3 > 4);
    operand.shift_ = Shift(Get()->imm3);
}

void A64_SUB_EXT_REG::Assemble() {
    SET_OPCODE_MULTI(SUB_EXT_REG, 1);
    SET_OPCODE_MULTI(SUB_EXT_REG, 2);
    Get()->S = flagsUpdate;
    Get()->sf = rd->isX() ? 1 : 0;
    Get()->option = operand.extend_;
    Get()->imm3 = operand.shift_;
    Get()->rm = operand.reg_->Code();
    ENCODE_RN;
    ENCODE_RD;
}


A64_EXCEPTION_GEN::A64_EXCEPTION_GEN(void* inst) : InstructionA64(inst) {
}


A64_EXCEPTION_GEN::A64_EXCEPTION_GEN(A64_EXCEPTION_GEN::OP op, ExceptionLevel el, U16 imme) : op(
        op), el(el), imme(imme) {}

void A64_EXCEPTION_GEN::Disassemble() {
    DECODE_OP;
    el = ExceptionLevel(Get()->ll);
    imme = static_cast<U16>(Get()->imm16);
}

void A64_EXCEPTION_GEN::Assemble() {
    SET_OPCODE_MULTI(EXCEPTION_GEN, 1);
    SET_OPCODE_MULTI(EXCEPTION_GEN, 2);
    ENCODE_OP;
    Get()->ll = el;
    Get()->imm16 = imme;
}


A64_SVC::A64_SVC(U16 imme) : A64_EXCEPTION_GEN(XXC, EL1,imme) {}

A64_SVC::A64_SVC(void *inst) : A64_EXCEPTION_GEN(inst) {}



A64_LDR_IMM::A64_LDR_IMM(void *inst) : A64LoadAndStoreImm(inst) {
}

A64_LDR_IMM::A64_LDR_IMM(RegisterA64 &rt, const MemOperand &operand) : A64LoadAndStoreImm(&rt,
                                                                                          operand) {}

void A64_LDR_IMM::Disassemble() {
    regSize = RegSize(Get()->size);
    switch (regSize) {
        case Size64:
            rt = XReg(static_cast<U8>(Get()->rt));
            operand.base_ = XReg(static_cast<U8>(Get()->rn));
            break;
        case Size32:
            rt = WReg(static_cast<U8>(Get()->rt));
            operand.base_ = WReg(static_cast<U8>(Get()->rn));
            break;
        default:
            valid_ = false;
            return;
    }
    addrMode = AdMod(Get()->addrmode);
    switch (addrMode) {
        case PostIndex:
            wback = true;
            postindex = true;
            operand.addr_mode_ = AddrMode::PostIndex;
            break;
        case PreIndex:
            wback = true;
            postindex = false;
            operand.addr_mode_ = AddrMode::PreIndex;
            break;
        default:
            operand.addr_mode_ = AddrMode::Offset;
            valid_ = false;
    }
    scale = static_cast<U8>(Get()->size);
    offset = SignExtend64(9, Get()->imm9);
    operand.offset_ = offset;
}

void A64_LDR_IMM::Assemble() {
    SET_OPCODE(LDR_IMM);
    Get()->rt = rt->Code();
    Get()->rn = operand.base_->Code();
    Get()->imm9 = TruncateToUint9(operand.offset_);
    if (rt->isX()) {
        Get()->size = Size64;
    } else if (rt->isW()) {
        Get()->size = Size32;
    } else {
        valid_ = false;
        return;
    }
    switch (operand.addr_mode_) {
        case AddrMode::PostIndex:
            wback = true;
            postindex = true;
            Get()->addrmode = PostIndex;
            break;
        case AddrMode::PreIndex:
            wback = true;
            postindex = false;
            Get()->addrmode = PreIndex;
            break;
        default:
            Get()->addrmode = Offset;
            valid_ = false;
    }
}




A64_LDR_UIMM::A64_LDR_UIMM(void *inst) : A64LoadAndStoreImm(inst) {
}

A64_LDR_UIMM::A64_LDR_UIMM(RegisterA64 &rt, const MemOperand &operand) : A64LoadAndStoreImm(&rt,
                                                                                            operand) {}

void A64_LDR_UIMM::Disassemble() {
    regSize = RegSize(Get()->size);
    switch (regSize) {
        case Size64:
            rt = XReg(static_cast<U8>(Get()->rt));
            operand.base_ = XReg(static_cast<U8>(Get()->rn));
            break;
        case Size32:
            rt = WReg(static_cast<U8>(Get()->rt));
            operand.base_ = WReg(static_cast<U8>(Get()->rn));
            break;
        default:
            valid_ = false;
            return;
    }
    operand.addr_mode_ = AddrMode::Offset;
    scale = static_cast<U8>(Get()->size);
    offset = Get()->imm12 << regSize;
    operand.offset_ = offset;
}

void A64_LDR_UIMM::Assemble() {
    SET_OPCODE(LDR_UIMM);
    Get()->rt = rt->Code();
    Get()->rn = operand.base_->Code();
    if (rt->isX()) {
        Get()->size = Size64;
    } else if (rt->isW()) {
        Get()->size = Size32;
    } else {
        valid_ = false;
        return;
    }
    Get()->imm12 = static_cast<InstA64>(operand.offset_ >> Get()->size);
}



A64_LDRSW_IMM::A64_LDRSW_IMM(void *inst) : A64_LDR_IMM(inst) {}

A64_LDRSW_IMM::A64_LDRSW_IMM(RegisterA64 &rt, const MemOperand &operand) : A64_LDR_IMM(rt,
                                                                                       operand) {}

void A64_LDRSW_IMM::Disassemble() {
    rt = XReg(static_cast<U8>(Get()->rt));
    addrMode = AdMod(Get()->addrmode);
    switch (addrMode) {
        case PostIndex:
            wback = true;
            postindex = true;
            operand.addr_mode_ = AddrMode::PostIndex;
            break;
        case PreIndex:
            wback = true;
            postindex = false;
            operand.addr_mode_ = AddrMode::PreIndex;
            break;
        default:
            valid_ = false;
            return;
    }
    scale = static_cast<U8>(Get()->size);
    offset = SignExtend64(9, Get()->imm9);
    operand.offset_ = offset;
    rt = XReg(static_cast<U8>(Get()->rt));
    operand.base_ = XReg(static_cast<U8>(Get()->rn));
}

void A64_LDRSW_IMM::Assemble() {
    SET_OPCODE(LDRSW_IMM);
    Get()->size = Size32;
    Get()->rt = rt->Code();
    Get()->rn = operand.base_->Code();
    Get()->imm9 = TruncateToUint9(operand.offset_);
    switch (operand.addr_mode_) {
        case AddrMode::PostIndex:
            wback = true;
            postindex = true;
            Get()->addrmode = PostIndex;
            break;
        case AddrMode::PreIndex:
            wback = true;
            postindex = false;
            Get()->addrmode = PreIndex;
            break;
        default:
            valid_ = false;
            return;
    }
}



A64_LDRSW_UIMM::A64_LDRSW_UIMM(void *inst) : A64_LDR_UIMM(inst) {}

A64_LDRSW_UIMM::A64_LDRSW_UIMM(XRegister &rt, const MemOperand &operand) : A64_LDR_UIMM(rt,
                                                                                        operand) {}

void A64_LDRSW_UIMM::Disassemble() {
    DECODE_RT(XReg);
    operand.base_ = XReg(static_cast<U8>(Get()->rn));
    operand.addr_mode_ = AddrMode::Offset;
    scale = static_cast<U8>(Get()->size);
    offset = Get()->imm12 << Size32;
    operand.offset_ = offset;
}

void A64_LDRSW_UIMM::Assemble() {
    SET_OPCODE(LDRSW_UIMM);
    Get()->size = Size32;
    ENCODE_RT;
    Get()->rn = operand.base_->Code();
    Get()->imm12 = operand.offset_ >> Size32;
}



A64_STP_LDP::A64_STP_LDP(void *inst) : InstructionA64(inst) {
}

A64_STP_LDP::A64_STP_LDP(OP op, RegisterA64 &rt1, RegisterA64 &rt2, const MemOperand &operand) : op(op), rt1(&rt1),
                                                                                                 rt2(&rt2),
                                                                                                 operand(operand) {}

void A64_STP_LDP::Disassemble() {
    DECODE_OP;
    RegSize s = RegSize(Get()->size);
    if (s == Size64) {
        rt1 = XReg(Get()->rt);
        rt2 = XReg(Get()->rt2);
        operand.offset_ = SignExtend64(7, Get()->imm7) << 3;
    } else {
        rt1 = WReg(Get()->rt);
        rt2 = WReg(Get()->rt2);
        operand.offset_ = SignExtend64(7, Get()->imm7) << 2;
    }
    operand.base_ = XReg(Get()->rn);
    AdMod adMod = AdMod(Get()->addrmode);
    switch (adMod) {
        case SignOffset:
            operand.addr_mode_ = AddrMode::Offset;
            break;
        case PostIndex:
            operand.addr_mode_ = AddrMode::PostIndex;
            break;
        case PreIndex:
            operand.addr_mode_ = AddrMode::PreIndex;
            break;
    }
}

void A64_STP_LDP::Assemble() {
    SET_OPCODE(STP_LDP);
    ENCODE_OP;
    Get()->size = rt1->isX() ? Size64 : Size32;
    Get()->rt = rt1->Code();
    Get()->rt2 = rt2->Code();
    Get()->rn = operand.base_->Code();
    switch (operand.addr_mode_) {
        case Offset:
            Get()->addrmode = SignOffset;
            break;
        case AddrMode::PostIndex:
            Get()->addrmode = PostIndex;
            break;
        case AddrMode::PreIndex:
            Get()->addrmode = PreIndex;
            break;
        default:
            valid_ = false;
    }
    Get()->imm7 = TruncateToUint7(operand.offset_ >> (rt1->isX() ? 3 : 2));
}


A64_ADD_SUB_IMM::A64_ADD_SUB_IMM(void *inst) : InstructionA64(inst) {
}

A64_ADD_SUB_IMM::A64_ADD_SUB_IMM(A64_ADD_SUB_IMM::OP op, A64_ADD_SUB_IMM::S sign, RegisterA64 &rd,
                                 const Operand &operand) : op(op), sign(sign), rd(&rd), operand(operand) {}

void A64_ADD_SUB_IMM::Disassemble() {
    DECODE_OP;
    if (Get()->sf == Size64) {
        DECODE_RD(XReg);
        operand.reg_ = XReg(Get()->rn);
    } else {
        DECODE_RD(XReg);
        operand.reg_ = WReg(Get()->rn);
    }
    sign = S(Get()->S);
    DECODE_SHIFT;
    if (operand.shift_ == LSL) {
        operand.shift_extend_imm_ = Get()->imm12;
    } else if (operand.shift_ == LSR) {
        operand.shift_extend_imm_ = Get()->imm12 << 12;
    } else {
        valid_ = false;
    }
}

void A64_ADD_SUB_IMM::Assemble() {
    SET_OPCODE(ADD_SUB_IMM);
    ENCODE_OP;
    ENCODE_RD;
    Get()->rn = operand.reg_->Code();
    Get()->sf = rd->isX() ? Size64 : Size32;
    Get()->S = sign;
    ENCODE_SHIFT;
    if (operand.shift_ == LSL) {
        Get()->imm12 = operand.shift_extend_imm_;
    } else if (operand.shift_ == LSR) {
        Get()->imm12 = operand.shift_extend_imm_ >> 12;
    } else {
        valid_ = false;
    }
}

A64_MSR_MRS::A64_MSR_MRS(void *inst) : InstructionA64(inst) {
}

A64_MSR_MRS::A64_MSR_MRS(OP op, SystemRegister &systemRegister, RegisterA64 &rt) : op(op), system_reg(
        systemRegister), rt(&rt) {}

void A64_MSR_MRS::Disassemble() {
    DECODE_OP;
    DECODE_RT(XReg);
    system_reg.value = static_cast<U16>(Get()->sysreg);
}

void A64_MSR_MRS::Assemble() {
    SET_OPCODE(MSR_MRS);
    ENCODE_OP;
    ENCODE_RT;
    Get()->sysreg = system_reg.value;
}
