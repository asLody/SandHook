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

T32_LDR_LIT::T32_LDR_LIT(T32_STRUCT_LDR_LIT *inst) : T32_INST_PC_REL(inst) {}

T32_LDR_LIT::T32_LDR_LIT(RegisterA32 &rt, Off offset) : rt(&rt), offset(offset) {}

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
