//
// Created by swift on 2019/5/12.
//

#pragma once


#include "instruction.h"

#define STRUCT_T32(X) T32_STRUCT_##X
#define OPCODE_T32(X) T32_OPCODE_##X
#define DEFINE_OPCODE_T32(X, V) const U32 OPCODE_T32(X) = V;
#define DEFINE_STRUCT_T32(X) struct STRUCT_T32(X) : public Base

#define T32_REG_WIDE 4


//Unknow inst
DEFINE_STRUCT_T32(UNKNOW) {
    InstT32 raw;
};


DEFINE_OPCODE_T32(B32, 0b11110)
DEFINE_STRUCT_T32(B32) {
    InstT32 imm10:10;
    InstT32 S:1;
    InstT32 opcode:5;
    InstT32 imm11:11;
    InstT32 J2:1;
    InstT32 X:1;
    InstT32 J1:1;
    InstT32 op:2;
};

DEFINE_OPCODE_T32(LDR_LIT, 0b1111100)
DEFINE_STRUCT_T32(LDR_LIT) {
    InstT32 op:7;
    InstT32 U:1;
    InstT32 S:1;
    InstT32 opcode:7;
    InstT32 imm12:12;
    InstT32 rt:T32_REG_WIDE;
};


//ldr imm T3
DEFINE_OPCODE_T32(LDR_UIMM, 0b111110001101)
DEFINE_STRUCT_T32(LDR_UIMM) {
    InstT32 rn:T32_REG_WIDE;
    InstT32 opcode:12;
    InstT32 imm12:12;
    InstT32 rt:T32_REG_WIDE;
};


//ldr imm T4
DEFINE_OPCODE_T32(LDR_IMM_1, 0b11111000)
DEFINE_OPCODE_T32(LDR_IMM_2, 0b1)
DEFINE_STRUCT_T32(LDR_IMM) {
    InstT32 rn:T32_REG_WIDE;
    InstT32 op:4;
    InstT32 opcode1:8;
    InstT32 imm8:8;
    InstT32 W:1;
    InstT32 U:1;
    InstT32 P:1;
    InstT32 opcode2:1;
    InstT32 rt:T32_REG_WIDE;
};

DEFINE_OPCODE_T32(MOV_MOVT_IMM_1, 0b11110)
DEFINE_OPCODE_T32(MOV_MOVT_IMM_2, 0b0)
DEFINE_STRUCT_T32(MOV_MOVT_IMM) {
    InstT32 imm4:4;
    InstT32 op:6;
    InstT32 i:1;
    InstT32 opcode1:5;
    InstT32 imm8:8;
    InstT32 rd:4;
    InstT32 imm3:3;
    InstT32 opcode2:1;
};

DEFINE_OPCODE_T32(SUB_IMM_1, 0b11110)
DEFINE_OPCODE_T32(SUB_IMM_2, 0b0)
DEFINE_STRUCT_T32(SUB_IMM) {
    InstT32 rn:T32_REG_WIDE;
    InstT32 S:1;
    InstT32 op:5;
    InstT32 i:1;
    InstT32 opcode1:5;
    InstT32 imm8:8;
    InstT32 rd:T32_REG_WIDE;
    InstT32 imm3:3;
    InstT32 opcode2:1;
};

DEFINE_OPCODE_T32(HVC_1, 0b111101111110)
DEFINE_OPCODE_T32(HVC_2, 0b1000)
DEFINE_STRUCT_T32(HVC) {
    InstT32 imm4:4;
    InstT32 opcode1:12;
    InstT32 imm12:12;
    InstT32 opcode2:4;
};
