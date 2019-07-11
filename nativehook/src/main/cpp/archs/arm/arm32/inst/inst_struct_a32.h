//
// Created by swift on 2019/5/12.
//

#pragma once


#include "instruction.h"

#define STRUCT_A32(X) A32_STRUCT_##X
#define OPCODE_A32(X) A32_OPCODE_##X
#define DEFINE_OPCODE(X, V) const U32 OPCODE_A32(X) = V;
#define DEFINE_STRUCT_A32(X) struct STRUCT_A32(X) : public Base


DEFINE_OPCODE(STR_IMM, 0b010)
DEFINE_STRUCT_A32(STR_IMM) {
        InstA64 imm12:12;
        InstA64 rt:4;
        InstA64 rn:4;
        InstA64 unkown2_0:1;
        InstA64 W:1;
        InstA64 unkown1_0:1;
        InstA64 U:1;
        InstA64 P:1;
        InstA64 opcode:3;
        InstA64 cond:4;
};