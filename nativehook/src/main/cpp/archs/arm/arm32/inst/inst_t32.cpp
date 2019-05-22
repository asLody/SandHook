//
// Created by swift on 2019/5/16.
//

#include "inst_t32.h"
#include "inst_struct_t32.h"


#define SET_BASE_OPCODE(X) get()->opcode_base = OPCODE_T32(X)

#define SET_OPCODE(X) get()->opcode = OPCODE_T32(X)
#define SET_OPCODE_MULTI(X, INDEX) get()->opcode##INDEX = OPCODE_T32(X##_##INDEX)

using namespace SandHook::Asm;
using namespace SandHook::AsmA32;


T32_B32::T32_B32(T32_STRUCT_B32 *inst) : T32_INST_PC_REL(inst) {}

T32_B32::T32_B32(T32_B32::OP op, T32_B32::X x, Off offset) : op(op), x(x), offset(offset) {}

T32_B32::T32_B32(T32_B32::OP op, T32_B32::X x, Label &label) : op(op), x(x) {
    bindLabel(label);
}

Off T32_B32::getImmPCOffset() {
    U32 S = get()->S;
    U32 imm21 = COMBINE(get()->imm10, get()->imm11, 11);
    U32 i1 = (get()->J1 ^ S) ? 0 : 1;
    U32 i2 = (get()->J2 ^ S) ? 0 : 1;
    U32 i1i2 = COMBINE(i1, i2, 1);
    U32 Si1i2 = COMBINE(S, i1i2, 2);
    //BLX
    if (op == BL && x == thumb) {
        INST_ASSERT((imm21 & 0x1) == 0x1);
    }
    return signExtend32(25, COMBINE(Si1i2, imm21, 21) << 0);
}

void T32_B32::decode(T32_STRUCT_B32 *inst) {
    DECODE_OP;
    x = X(inst->X);
    offset = getImmPCOffset();
}

void T32_B32::assembler() {
    SET_OPCODE(B32);
    ENCODE_OP;
    get()->X = x;
    U32 imm24 = TruncateToUint25(offset) >> 0;
    get()->imm11 = BITS(imm24, 0, 10);
    get()->imm10 = BITS(imm24, 11, 20);
    get()->S = BIT(imm24, 23);
    U32 i1 = BIT(imm24, 22);
    U32 i2 = BIT(imm24, 21);
    if (i1 == 1) {
        get()->J1 = get()->S;
    } else {
        get()->J1 = ~get()->S;
    }
    if (i2 == 1) {
        get()->J2 = get()->S;
    } else {
        get()->J2 = ~get()->S;
    }
}



T32_LDR_UIMM::T32_LDR_UIMM(T32_STRUCT_LDR_UIMM *inst) : InstructionT32(inst) {}

T32_LDR_UIMM::T32_LDR_UIMM(RegisterA32 &rt, RegisterA32 &rn, U32 offset) : rt(&rt), rn(&rn),
                                                                           offset(offset) {}

void T32_LDR_UIMM::decode(T32_STRUCT_LDR_UIMM *inst) {
    DECODE_RN(Reg);
    DECODE_RT(Reg);
    INST_ASSERT(rn == &PC);
    offset = inst->imm12;
}

void T32_LDR_UIMM::assembler() {
    SET_OPCODE(LDR_UIMM);
    ENCODE_RN;
    ENCODE_RT;
    INST_ASSERT(rn == &PC);
    get()->imm12 = offset;
}

T32_LDR_LIT::T32_LDR_LIT() {}

T32_LDR_LIT::T32_LDR_LIT(T32_STRUCT_LDR_LIT *inst) : T32_INST_PC_REL(inst) {
    decode(inst);
}

T32_LDR_LIT::T32_LDR_LIT(OP op, RegisterA32 &rt, Off offset) : op(op), rt(&rt), offset(offset) {}


T32_LDR_LIT::T32_LDR_LIT(OP op, RegisterA32 &rt, Label& label) : op(op), rt(&rt) {
    bindLabel(label);
}

Off T32_LDR_LIT::getImmPCOffset() {
    return get()->U == add ? get()->imm12 : -get()->imm12;
}

void T32_LDR_LIT::decode(T32_STRUCT_LDR_LIT *inst) {
    DECODE_OP;
    DECODE_RT(Reg);
    offset = getImmPCOffset();
}

void T32_LDR_LIT::assembler() {
    SET_OPCODE(LDR_LIT);
    ENCODE_OP;
    ENCODE_RT;
    if (offset >= 0) {
        get()->U = add;
        get()->imm12 = static_cast<InstT32>(offset);
    } else {
        get()->U = cmp;
        get()->imm12 = static_cast<InstT32>(-offset);
    }
}



T32_MOV_MOVT_IMM::T32_MOV_MOVT_IMM() {}

T32_MOV_MOVT_IMM::T32_MOV_MOVT_IMM(T32_STRUCT_MOV_MOVT_IMM *inst) : InstructionT32(inst) {}

T32_MOV_MOVT_IMM::T32_MOV_MOVT_IMM(T32_MOV_MOVT_IMM::OP op, RegisterA32 &rd, U16 imm16) : op(op),
                                                                                          rd(&rd),
                                                                                          imm16(imm16) {}

void T32_MOV_MOVT_IMM::decode(T32_STRUCT_MOV_MOVT_IMM *inst) {
    DECODE_RD(Reg);
    U16 imm4i = COMBINE(inst->imm4, inst->i, 1);
    U16 imm38 = COMBINE(inst->imm3, inst->imm8, 8);
    imm16 = COMBINE(imm4i, imm38, 11);
}

void T32_MOV_MOVT_IMM::assembler() {
    SET_OPCODE_MULTI(MOV_MOVT_IMM, 1);
    SET_OPCODE_MULTI(MOV_MOVT_IMM, 2);
    ENCODE_RD;
    get()->imm8 = BITS(imm16, 0, 7);
    get()->imm3 = BITS(imm16, 8 ,10);
    get()->i = BIT(imm16, 11);
    get()->imm4 = BITS(imm16, 12, 15);
}


T32_LDR_IMM::T32_LDR_IMM(T32_STRUCT_LDR_IMM *inst) : InstructionT32(inst) {
    decode(inst);
}

T32_LDR_IMM::T32_LDR_IMM(T32_LDR_IMM::OP op, RegisterA32 &rt, const MemOperand &operand) : op(op),
                                                                                           rt(&rt),
                                                                                           operand(operand) {}

void T32_LDR_IMM::decode(T32_STRUCT_LDR_IMM *inst) {
    DECODE_OP;
    DECODE_RT(Reg);
    operand.rn = Reg(static_cast<U8>(inst->rn));
    if (inst->P == 1 && inst->U == 0 && inst->W == 0) {
        operand.addr_mode = Offset;
    } else if (inst->P == 0 && inst->W == 1) {
        operand.addr_mode = PostIndex;
    } else if (inst->P == 1 && inst->W == 1) {
        operand.addr_mode = PreIndex;
    } else {
        valid = false;
    }
    operand.offset = inst->U == 0 ? -inst->imm8 : inst->imm8;
}

void T32_LDR_IMM::assembler() {
    SET_OPCODE_MULTI(LDR_IMM, 1);
    SET_OPCODE_MULTI(LDR_IMM, 2);
    ENCODE_OP;
    get()->rn = operand.rn->getCode();
    if (operand.offset < 0) {
        get()->imm8 = static_cast<InstT32>(-operand.offset);
        get()->U = 0;
    } else {
        get()->imm8 = static_cast<InstT32>(operand.offset);
        get()->U = 1;
    }
    switch (operand.addr_mode) {
        case Offset:
            get()->P = 1;
            get()->U = 0;
            get()->W = 0;
            break;
        case PostIndex:
            get()->P = 0;
            get()->W = 1;
            break;
        case PreIndex:
            get()->P = 1;
            get()->W = 1;
            break;
        default:
            valid = false;
    }
}
