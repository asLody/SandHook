//
// Created by swift on 2019/5/12.
//

#pragma once

#include "instruction.h"
#include "inst_code_arm32.h"

#define STRUCT_T16(X) T16_STRUCT_##X
#define OPCODE_T16(X) T16_OPCODE_##X
#define DEFINE_OPCODE_T16(X, V) const U32 OPCODE_T16(X) = V;
#define DEFINE_STRUCT_T16(X) struct STRUCT_T16(X) : public Base

#define DEFINE_STRCUT_BASE_T16(w_base, w_op) \
    InstT16 opcode:w_op;    \
    InstT16 opcode_base:w_base;


//Shift (immediate_), add, subtract, Move, and compare
//opcode_base == 0b00
DEFINE_OPCODE_T16(BASE_SASMC, 0b00)
//Data-processing
DEFINE_OPCODE_T16(DATA_PROC, 0b010000)

DEFINE_OPCODE_T16(SPDIABE, 0b010001)

DEFINE_OPCODE_T16(MISC, 0b1011)

#define DATA_PROC_FIELDS DEFINE_STRCUT_BASE_T16(6, 4)

#define T16_REG_WIDE 3


//Unknow inst
DEFINE_STRUCT_T16(UNKNOW) {
    InstT16 raw;
};

DEFINE_OPCODE_T16(B, 0b11100)
DEFINE_STRUCT_T16(B) {
    InstT16 imm11:11;
    InstT16 opcode:5;
};

DEFINE_OPCODE_T16(B_COND, 0b1101)
DEFINE_STRUCT_T16(B_COND) {
    InstT16 imm8:8;
    InstT16 cond:4;
    InstT16 opcode:4;
};


DEFINE_OPCODE_T16(BX_BLX_1, 0b01000111)
DEFINE_OPCODE_T16(BX_BLX_2, 0b000)
DEFINE_STRUCT_T16(BX_BLX) {
    InstT16 opcode2:3;
    InstT16 rm:4;
    InstT16 op:1;
    InstT16 opcode1:8;
};

DEFINE_OPCODE_T16(CBZ_CBNZ_1, 0b1011)
DEFINE_OPCODE_T16(CBZ_CBNZ_2, 0b0)
DEFINE_OPCODE_T16(CBZ_CBNZ_3, 0b1)
DEFINE_STRUCT_T16(CBZ_CBNZ) {
    InstT16 rn:T16_REG_WIDE;
    InstT16 imm5:5;
    InstT16 opcode3:1;
    InstT16 i:1;
    InstT16 opcode2:1;
    InstT16 op:1;
    InstT16 opcode1:4;
};

DEFINE_OPCODE_T16(LDR_LIT, 0b01001)
DEFINE_STRUCT_T16(LDR_LIT) {
    InstT16 imm8:8;
    InstT16 rt:T16_REG_WIDE;
    InstT16 opcode:5;
};


DEFINE_OPCODE_T16(ADD_IMM_RDN, 0b00110)
DEFINE_STRUCT_T16(ADD_IMM_RDN) {
    InstT16 imm8:8;
    InstT16 rdn:T16_REG_WIDE;
    InstT16 opcode:5;
};


DEFINE_OPCODE_T16(ADR, 0b10100)
DEFINE_STRUCT_T16(ADR) {
    InstT16 imm8:8;
    InstT16 rd:T16_REG_WIDE;
    InstT16 opcode:5;
};

DEFINE_OPCODE_T16(CMP_REG, 0b1010)
DEFINE_STRUCT_T16(CMP_REG) {
    InstT16 rn:T16_REG_WIDE;
    InstT16 rm:T16_REG_WIDE;
    DATA_PROC_FIELDS
};

DEFINE_OPCODE_T16(CMP_REG_EXT, 0b1010)
DEFINE_STRUCT_T16(CMP_REG_EXT) {
    InstT16 rn:4;
    InstT16 rm:4;
    InstT16 N:1;
    InstT16 opcode:8;
};

DEFINE_OPCODE_T16(MOV_REG, 0b10)
DEFINE_STRUCT_T16(MOV_REG) {
    InstT16 rd:T16_REG_WIDE;
    InstT16 rm:4;
    InstT16 D:1;
    DEFINE_STRCUT_BASE_T16(6, 2)
};

DEFINE_OPCODE_T16(MOV_IMM, 0b00100)
DEFINE_STRUCT_T16(MOV_IMM) {
    InstT16 imm8:8;
    InstT16 rd:T16_REG_WIDE;
    InstT16 opcode:5;
};

DEFINE_OPCODE_T16(ADD_REG, 0b0001100)
DEFINE_STRUCT_T16(ADD_REG) {
    InstT16 rd:T16_REG_WIDE;
    InstT16 rn:T16_REG_WIDE;
    InstT16 rm:T16_REG_WIDE;
    InstT16 opcode:7;
};

DEFINE_OPCODE_T16(ADD_REG_RDN, 0b01000100)
DEFINE_STRUCT_T16(ADD_REG_RDN) {
    InstT16 rdn:T16_REG_WIDE;
    InstT16 rm:4;
    InstT16 unknow:1;
    InstT16 opcode:8;
};

DEFINE_OPCODE_T16(POP, 0b1011110)
DEFINE_STRUCT_T16(POP) {
    InstT16 regs:8;
    InstT16 P:1;
    InstT16 opcode:7;
};

DEFINE_OPCODE_T16(PUSH, 0b1011010)
DEFINE_STRUCT_T16(PUSH) {
    InstT16 regs:8;
    InstT16 M:1;
    InstT16 opcode:7;
};