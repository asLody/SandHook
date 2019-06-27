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

T32_UNKNOW::T32_UNKNOW(STRUCT_T32(UNKNOW) &inst) : InstructionT32(&inst) {
    Disassembler(&inst);
}

void T32_UNKNOW::Disassembler(T32_STRUCT_UNKNOW *inst) {
    inst_backup = *inst;
}

void T32_UNKNOW::Assembler() {
    Set(inst_backup);
}


T32_B32::T32_B32(T32_STRUCT_B32 *inst) : T32_INST_PC_REL(inst) {
    Disassembler(inst);
}

T32_B32::T32_B32(T32_B32::OP op, T32_B32::X x, Off offset) : op(op), x(x), offset(offset) {}

T32_B32::T32_B32(T32_B32::OP op, T32_B32::X x, Label &label) : op(op), x(x) {
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

void T32_B32::Disassembler(T32_STRUCT_B32 *inst) {
    DECODE_OP;
    x = X(inst->X);
    offset = GetImmPCOffset();
}

void T32_B32::Assembler() {
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
        void* base = reinterpret_cast<void *>(ALIGN((Addr) getPC(), 4));
        return offset + reinterpret_cast<Addr>(base);
    } else {
        return T32_INST_PC_REL::GetImmPCOffsetTarget();
    }
}


T32_LDR_UIMM::T32_LDR_UIMM(T32_STRUCT_LDR_UIMM *inst) : InstructionT32(inst) {}

T32_LDR_UIMM::T32_LDR_UIMM(RegisterA32 &rt, RegisterA32 &rn, U32 offset) : rt(&rt), rn(&rn),
                                                                           offset(offset) {}

void T32_LDR_UIMM::Disassembler(T32_STRUCT_LDR_UIMM *inst) {
    DECODE_RN(Reg);
    DECODE_RT(Reg);
    INST_ASSERT(rn == &PC);
    offset = inst->imm12;
}

void T32_LDR_UIMM::Assembler() {
    SET_OPCODE(LDR_UIMM);
    ENCODE_RN;
    ENCODE_RT;
    INST_ASSERT(rn == &PC);
    Get()->imm12 = offset;
}

T32_LDR_LIT::T32_LDR_LIT() {}

T32_LDR_LIT::T32_LDR_LIT(T32_STRUCT_LDR_LIT *inst) : T32_INST_PC_REL(inst) {
    Disassembler(inst);
}

T32_LDR_LIT::T32_LDR_LIT(OP op, S s, RegisterA32 &rt, Off offset) : op(op), s(s), rt(&rt), offset(offset) {}


T32_LDR_LIT::T32_LDR_LIT(OP op, S s, RegisterA32 &rt, Label& label) : op(op), s(s), rt(&rt) {
    BindLabel(label);
}

Off T32_LDR_LIT::GetImmPCOffset() {
    return Get()->U == add ? Get()->imm12 : -Get()->imm12;
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

void T32_LDR_LIT::Disassembler(T32_STRUCT_LDR_LIT *inst) {
    DECODE_OP;
    DECODE_RT(Reg);
    s = S(inst->S);
    offset = GetImmPCOffset();
}

void T32_LDR_LIT::Assembler() {
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


T32_MOV_MOVT_IMM::T32_MOV_MOVT_IMM() {}

T32_MOV_MOVT_IMM::T32_MOV_MOVT_IMM(T32_STRUCT_MOV_MOVT_IMM *inst) : InstructionT32(inst) {
    Disassembler(inst);
}

T32_MOV_MOVT_IMM::T32_MOV_MOVT_IMM(T32_MOV_MOVT_IMM::OP op, RegisterA32 &rd, U16 imm16) : op(op),
                                                                                          rd(&rd),
                                                                                          imm16(imm16) {}

void T32_MOV_MOVT_IMM::Disassembler(T32_STRUCT_MOV_MOVT_IMM *inst) {
    DECODE_OP;
    DECODE_RD(Reg);
    U16 imm4i = COMBINE(inst->imm4, inst->i, 1);
    U16 imm38 = COMBINE(inst->imm3, inst->imm8, 8);
    imm16 = COMBINE(imm4i, imm38, 11);
}

void T32_MOV_MOVT_IMM::Assembler() {
    SET_OPCODE_MULTI(MOV_MOVT_IMM, 1);
    SET_OPCODE_MULTI(MOV_MOVT_IMM, 2);
    ENCODE_OP;
    ENCODE_RD;
    Get()->imm8 = BITS(imm16, 0, 7);
    Get()->imm3 = BITS(imm16, 8 ,10);
    Get()->i = BIT(imm16, 11);
    Get()->imm4 = BITS(imm16, 12, 15);
}


T32_LDR_IMM::T32_LDR_IMM(T32_STRUCT_LDR_IMM *inst) : InstructionT32(inst) {
    Disassembler(inst);
}

T32_LDR_IMM::T32_LDR_IMM(T32_LDR_IMM::OP op, RegisterA32 &rt, const MemOperand &operand) : op(op),
                                                                                           rt(&rt),
                                                                                           operand(operand) {}

void T32_LDR_IMM::Disassembler(T32_STRUCT_LDR_IMM *inst) {
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

void T32_LDR_IMM::Assembler() {
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
            valid = false;
    }
}



T32_SUB_IMM::T32_SUB_IMM(T32_STRUCT_SUB_IMM *inst) : InstructionT32(inst) {
    DisAssembler(inst);
}
