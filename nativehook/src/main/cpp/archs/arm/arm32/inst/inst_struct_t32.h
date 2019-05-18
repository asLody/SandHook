//
// Created by swift on 2019/5/12.
//

#ifndef SANDHOOK_NH_INST_STRUCT_T32_H
#define SANDHOOK_NH_INST_STRUCT_T32_H


#include "instruction.h"

#define STRUCT_T32(X) T32_STRUCT_##X
#define OPCODE_T32(X) T32_OPCODE_##X
#define DEFINE_OPCODE_T32(X, V) const U32 OPCODE_T32(X) = V;
#define DEFINE_STRUCT_T32(X) struct STRUCT_T32(X) : public Base


DEFINE_OPCODE_T32(B32, 0b11110)
DEFINE_STRUCT_T32(B32) {
    InstT32 imm11:11;
    InstT32 J2:1;
    InstT32 X:1;
    InstT32 J1:1;
    InstT32 op:2;
    InstT32 imm10:10;
    InstT32 S:1;
    InstT32 opcode:5;
};


#endif //SANDHOOK_NH_INST_STRUCT_T32_H
