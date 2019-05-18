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
    //U16 immhi =
}

void T32_B32::decode(T32_STRUCT_B32 *inst) {
    DECODE_OP;
    x = X(inst->X);
}

void T32_B32::assembler() {
    SET_OPCODE(B32);
    ENCODE_OP;
    get()->X = x;
}
