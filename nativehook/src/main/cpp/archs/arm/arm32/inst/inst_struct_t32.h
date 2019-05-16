//
// Created by swift on 2019/5/12.
//

#ifndef SANDHOOK_NH_INST_STRUCT_T16_H
#define SANDHOOK_NH_INST_STRUCT_T16_H


#include "instruction.h"

#define STRUCT_T32(X) T32_STRUCT_##X
#define OPCODE_T32(X) T32_OPCODE_##X
#define DEFINE_OPCODE_T32(X, V) const U32 OPCODE_T32(X) = V;
#define DEFINE_STRUCT_T32(X) struct STRUCT_T32(X) : public Base


DEFINE_OPCODE_T32(B, 0b010)
DEFINE_STRUCT_T32(B) {
};


#endif //SANDHOOK_NH_INST_STRUCT_T16_H
