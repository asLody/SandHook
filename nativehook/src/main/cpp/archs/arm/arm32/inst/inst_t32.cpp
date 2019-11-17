//
// Created by swift on 2019/5/16.
//

#include "inst_t32.h"
#include "inst_struct_t32.h"


#define SET_BASE_OPCODE(X) Get()->opcode_base = OPCODE_T32(X)

#define SET_OPCODE(X) Get()->opcode = OPCODE_T32(X)
#define SET_OPCODE_MULTI(X, INDEX) Get()->opcode##INDEX = OPCODE_T32(X##_##INDEX)

using namespace SandHook::Asm;
using namespace SandHook::AsmA32;



//Unknow

T32_UNKNOW::T32_UNKNOW(void *inst) : InstructionT32(inst) {
}



T32_B32::T32_B32(void *inst) : T32_INST_PC_REL(inst) {
}

T32_B32::T32_B32(T32_B32::OP op, T32_B32::X x, Off offset) : op(op), x(x), offset(offset) {}

T32_B32::T32_B32(T32_B32::OP op, T32_B32::X x, Label *label) : op(op), x(x) {
    BindLabel(label);
}

Off T32_B32::GetImmPCOffset() {
    U32 S = Get()->S;
    U32 imm21 = COMBINE(Get()->imm10, Get()->imm11, 11);
    if (Get()->X == 0 && op == BL) {
        imm21 &= ~(1 << 0);
    }
    U32 i1 = !(Get()->J1 ^ S);
    U32 i2 = !(Get()->J2 ^ S);
    U32 i1i2 = COMBINE(i1, i2, 1);
    U32 Si1i2 = COMBINE(-S, i1i2, 2);
    return SignExtend32(25, COMBINE(Si1i2, imm21, 21) << 1);
}

void T32_B32::Disassemble() {
    DECODE_OP;
    x = X(Get()->X);
    offset = GetImmPCOffset();
}

void T32_B32::Assemble() {
    SET_OPCODE(B32);
    ENCODE_OP;
    Get()->X = x;
    U32 imm24 = TruncateToUint25(offset) >> 1;
    Get()->imm11 = BITS(imm24, 0, 10);
    Get()->imm10 = BITS(imm24, 11, 20);
    if (Get()->X == 0) {
        Get()->imm11 |= (1 << 0);
    }
    Get()->S = BIT(imm24, 23);
    U32 i1 = BIT(imm24, 22);
    U32 i2 = BIT(imm24, 21);
    if (i1 == 1) {
        Get()->J1 = Get()->S;
    } else {
        Get()->J1 = !Get()->S;
    }
    if (i2 == 1) {
        Get()->J2 = Get()->S;
    } else {
        Get()->J2 = !Get()->S;
    }
}

Addr T32_B32::GetImmPCOffsetTarget() {
    if (x == arm && op == BL) {
        void* base = reinterpret_cast<void *>(ALIGN((Addr) GetPC(), 4));
        return offset + reinterpret_cast<Addr>(base);
    } else {
        return T32_INST_PC_REL::GetImmPCOffsetTarget();
    }
}


T32_LDR_UIMM::T32_LDR_UIMM(void *inst) : InstructionT32(inst) {}

T32_LDR_UIMM::T32_LDR_UIMM(RegisterA32 &rt, RegisterA32 &rn, U32 offset) : rt(&rt), rn(&rn),
                                                                           offset(offset) {}

void T32_LDR_UIMM::Disassemble() {
    DECODE_RN(Reg);
    DECODE_RT(Reg);
    INST_ASSERT(rn == &PC);
    offset = Get()->imm12;
}

void T32_LDR_UIMM::Assemble() {
    SET_OPCODE(LDR_UIMM);
    ENCODE_RN;
    ENCODE_RT;
    INST_ASSERT(rn == &PC);
    Get()->imm12 = offset;
}

T32_LDR_LIT::T32_LDR_LIT(void *inst) : T32_INST_PC_REL(inst) {
}

T32_LDR_LIT::T32_LDR_LIT(OP op, S s, RegisterA32 &rt, Off offset) : op(op), s(s), rt(&rt), offset(offset) {}


T32_LDR_LIT::T32_LDR_LIT(OP op, S s, RegisterA32 &rt, Label* label) : op(op), s(s), rt(&rt) {
    BindLabel(label);
}

Off T32_LDR_LIT::GetImmPCOffset() {
    return Get()->U == add ? Get()->imm12 : -Get()->imm12;
}

Addr T32_LDR_LIT::GetImmPCOffsetTarget() {
    return ALIGN((Addr) GetPC() + offset, 4);
}

void T32_LDR_LIT::OnOffsetApply(Off offset) {
    this->offset = offset;
    if (offset >= 0) {
        Get()->U = add;
        Get()->imm12 = static_cast<InstT32>(offset);
    } else {
        Get()->U = cmp;
        Get()->imm12 = static_cast<InstT32>(-offset);
    }
}

void T32_LDR_LIT::Disassemble() {
    DECODE_OP;
    DECODE_RT(Reg);
    s = S(Get()->S);
    offset = GetImmPCOffset();
}

void T32_LDR_LIT::Assemble() {
    SET_OPCODE(LDR_LIT);
    ENCODE_OP;
    ENCODE_RT;
    Get()->S = s;
    if (offset >= 0) {
        Get()->U = add;
        Get()->imm12 = static_cast<InstT32>(offset);
    } else {
        Get()->U = cmp;
        Get()->imm12 = static_cast<InstT32>(-offset);
    }
}


T32_MOV_MOVT_IMM::T32_MOV_MOVT_IMM(void *inst) : InstructionT32(inst) {
}

T32_MOV_MOVT_IMM::T32_MOV_MOVT_IMM(T32_MOV_MOVT_IMM::OP op, RegisterA32 &rd, U16 imm16) : op(op),
                                                                                          rd(&rd),
                                                                                          imm16(imm16) {}

void T32_MOV_MOVT_IMM::Disassemble() {
    DECODE_OP;
    DECODE_RD(Reg);
    U16 imm4i = COMBINE(Get()->imm4, Get()->i, 1);
    U16 imm38 = COMBINE(Get()->imm3, Get()->imm8, 8);
    imm16 = COMBINE(imm4i, imm38, 11);
}

void T32_MOV_MOVT_IMM::Assemble() {
    SET_OPCODE_MULTI(MOV_MOVT_IMM, 1);
    SET_OPCODE_MULTI(MOV_MOVT_IMM, 2);
    ENCODE_OP;
    ENCODE_RD;
    Get()->imm8 = BITS(imm16, 0, 7);
    Get()->imm3 = BITS(imm16, 8 ,10);
    Get()->i = BIT(imm16, 11);
    Get()->imm4 = BITS(imm16, 12, 15);
}


T32_LDR_IMM::T32_LDR_IMM(void *inst) : InstructionT32(inst) {
}

T32_LDR_IMM::T32_LDR_IMM(T32_LDR_IMM::OP op, RegisterA32 &rt, const MemOperand &operand) : op(op),
                                                                                           rt(&rt),
                                                                                           operand(operand) {}

void T32_LDR_IMM::Disassemble() {
    DECODE_OP;
    DECODE_RT(Reg);
    operand.rn = Reg(static_cast<U8>(Get()->rn));
    if (Get()->P == 1 && Get()->U == 0 && Get()->W == 0) {
        operand.addr_mode = Offset;
    } else if (Get()->P == 0 && Get()->W == 1) {
        operand.addr_mode = PostIndex;
    } else if (Get()->P == 1 && Get()->W == 1) {
        operand.addr_mode = PreIndex;
    } else {
        valid_ = false;
    }
    operand.offset = Get()->U == 0 ? -Get()->imm8 : Get()->imm8;
}

void T32_LDR_IMM::Assemble() {
    SET_OPCODE_MULTI(LDR_IMM, 1);
    SET_OPCODE_MULTI(LDR_IMM, 2);
    ENCODE_OP;
    Get()->rn = operand.rn->Code();
    if (operand.offset < 0) {
        Get()->imm8 = static_cast<InstT32>(-operand.offset);
        Get()->U = 0;
    } else {
        Get()->imm8 = static_cast<InstT32>(operand.offset);
        Get()->U = 1;
    }
    switch (operand.addr_mode) {
        case Offset:
            Get()->P = 1;
            Get()->U = 0;
            Get()->W = 0;
            break;
        case PostIndex:
            Get()->P = 0;
            Get()->W = 1;
            break;
        case PreIndex:
            Get()->P = 1;
            Get()->W = 1;
            break;
        default:
            valid_ = false;
    }
}



T32_SUB_IMM::T32_SUB_IMM(void *inst) : InstructionT32(inst) {
}


T32_HVC::T32_HVC(void *inst) : InstructionT32(inst) {}

T32_HVC::T32_HVC(U16 imme) : imme(imme) {}

void T32_HVC::Disassemble() {
    imme = static_cast<U16>(COMBINE(Get()->imm4, Get()->imm12, 12));
}

void T32_HVC::Assemble() {
    SET_OPCODE_MULTI(HVC, 1);
    SET_OPCODE_MULTI(HVC, 2);
    Get()->imm12 = BITS(imme, 0, 11);
    Get()->imm4 = BITS(imme, 12, 15);
}
