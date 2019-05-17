//
// Created by swift on 2019/5/16.
//

#include "inst_t16.h"
#include "arm32_base.h"

#define SET_BASE_OPCODE(X) get()->opcode_base = OPCODE_T16(X)

#define SET_OPCODE(X) get()->opcode = OPCODE_T16(X)
#define SET_OPCODE_MULTI(X, INDEX) get()->opcode##INDEX = OPCODE_T16(X##_##INDEX)

using namespace SandHook::AsmA32;



//B
T16_B::T16_B() {}

T16_B::T16_B(T16_STRUCT_B *inst) : T16_INST_PC_REL(inst) {
    decode(inst);
}

T16_B::T16_B(Off offset) : offset(offset) {}

T16_B::T16_B(Label &label) {
    bindLabel(label);
}

Off T16_B::getImmPCOffset() {
    return signExtend32(11 + 1, COMBINE(get()->imm11, 0, 1));
}

void T16_B::decode(T16_STRUCT_B *inst) {
    offset = getImmPCOffset();
}

void T16_B::assembler() {
    SET_OPCODE(B);
    get()->imm11 = TruncateToUint11(offset >> 1);
}

void T16_B::onOffsetApply(Off offset) {
    this->offset = offset;
    get()->imm11 = TruncateToUint11(offset >> 1);
}



//B Cond
T16_B_COND::T16_B_COND() {}

T16_B_COND::T16_B_COND(STRUCT_T16(B_COND) *inst) : T16_INST_PC_REL(inst) {
    decode(inst);
}

T16_B_COND::T16_B_COND(Condition condition, Off offset) : condition(condition), offset(offset) {}

T16_B_COND::T16_B_COND(Condition condition, Label &label) {
    bindLabel(label);
}

void T16_B_COND::decode(STRUCT_T16(B_COND) *inst) {
    DECODE_COND;
    offset = getImmPCOffset();
}

void T16_B_COND::assembler() {
    SET_OPCODE(B_COND);
    ENCODE_COND;
    ENCODE_OFFSET(8, 1);
}

void T16_B_COND::onOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(8, 1);
}

Off T16_B_COND::getImmPCOffset() {
    return DECODE_OFFSET(8, 1);
}



//BX BLX
T16_BX_BLX::T16_BX_BLX(T16_BX_BLX::OP op, RegisterA32 &rm) : op(op), rm(&rm) {}

T16_BX_BLX::T16_BX_BLX(T16_STRUCT_BX_BLX *inst) : T16_INST_PC_REL(inst) {
    decode(inst);
}

void T16_BX_BLX::decode(STRUCT_T16(BX_BLX) *inst) {
    DECODE_OP;
    DECODE_RM(Reg);
}

void T16_BX_BLX::assembler() {
    SET_OPCODE_MULTI(BX_BLX, 1);
    SET_OPCODE_MULTI(BX_BLX, 2);
    ENCODE_OP;
    ENCODE_RM;
}


//CBZ CBNZ
T16_CBZ_CBNZ::T16_CBZ_CBNZ(T16_STRUCT_CBZ_CBNZ *inst) : T16_INST_PC_REL(inst) {
    decode(inst);
}

T16_CBZ_CBNZ::T16_CBZ_CBNZ(T16_CBZ_CBNZ::OP op, Off offset, RegisterA32 &rn) : op(op), offset(offset),
                                                                              rn(&rn) {}


T16_CBZ_CBNZ::T16_CBZ_CBNZ(T16_CBZ_CBNZ::OP op, Label& label, RegisterA32 &rn) : op(op),
                                                                               rn(&rn) {
    bindLabel(label);
}



Off T16_CBZ_CBNZ::getImmPCOffset() {
    return COMBINE(get()->i, get()->imm5, 5) << 2;
}

void T16_CBZ_CBNZ::decode(T16_STRUCT_CBZ_CBNZ *inst) {
    offset = getImmPCOffset();
    DECODE_RN(Reg);
    DECODE_OP;
}

void T16_CBZ_CBNZ::assembler() {
    SET_OPCODE_MULTI(CBZ_CBNZ, 1);
    SET_OPCODE_MULTI(CBZ_CBNZ, 2);
    SET_OPCODE_MULTI(CBZ_CBNZ, 3);
    ENCODE_OP;
    ENCODE_RN;
    ENCODE_OFFSET(5, 2);
}

void T16_CBZ_CBNZ::onOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(5, 2);
}


//LDR_LIT
T16_LDR_LIT::T16_LDR_LIT(T16_STRUCT_LDR_LIT *inst) : T16_INST_PC_REL(inst) {
    decode(inst);
}

T16_LDR_LIT::T16_LDR_LIT(Off offset, RegisterA32 &rt) : offset(offset), rt(&rt) {

}

Off T16_LDR_LIT::getImmPCOffset() {
    return DECODE_OFFSET(8, 2);
}

Addr T16_LDR_LIT::getImmPCOffsetTarget() {
    return RoundDown((Addr) getPC() + offset, 4);
}

void T16_LDR_LIT::onOffsetApply(Off offset) {
    this->offset = offset;
    ENCODE_OFFSET(8, 2);
}

void T16_LDR_LIT::decode(T16_STRUCT_LDR_LIT *inst) {
    DECODE_RT(Reg);
    offset = getImmPCOffset();
}

void T16_LDR_LIT::assembler() {
    SET_OPCODE(LDR_LIT);
    ENCODE_RT;
}
