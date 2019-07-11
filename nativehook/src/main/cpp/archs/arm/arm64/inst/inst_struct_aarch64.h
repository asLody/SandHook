//
// Created by swift on 2019/5/5.
//

#pragma once


#include "instruction.h"
#include "inst_code_arm64.h"
#include "arm_base.h"

#define STRUCT_A64(X) A64_STRUCT_##X
#define OPCODE_A64(X) A64_OPCODE_##X
#define DEFINE_OPCODE_A64(X, V) const U32 OPCODE_A64(X) = V;
#define DEFINE_STRUCT_A64(X) struct STRUCT_A64(X) : public Base

enum ImmBranchType {
    UnknownBranchType = 0,
    CondBranchType = 1,
    UncondBranchType = 2,
    CompareBranchType = 3,
    TestBranchType = 4
};

enum Extend {
    NO_EXTEND = -1,
    UXTB      = 0,
    UXTH      = 1,
    UXTW      = 2,
    UXTX      = 3,
    SXTB      = 4,
    SXTH      = 5,
    SXTW      = 6,
    SXTX      = 7
};

enum FieldWide {
    WideReg = 5,
};

//Unknow inst
DEFINE_STRUCT_A64(UNKNOW) {
    InstA64 raw;
};

#define IMM_LO_W 2
#define IMM_HI_W 19
DEFINE_OPCODE_A64(ADR_ADRP, 0b10000)
struct STRUCT_A64(ADR_ADRP) {
    InstA64 rd:WideReg;
    InstA64 immhi:IMM_HI_W;
    InstA64 opcode:5;
    InstA64 immlo:IMM_LO_W;
    InstA64 op:1;
};

DEFINE_OPCODE_A64(MOV_WIDE, 0b100101)
DEFINE_STRUCT_A64(MOV_WIDE) {
    InstA64 rd:WideReg;
    InstA64 imm16:16;
    InstA64 hw:2;
    InstA64 opcode:6;
    InstA64 op:2;
    InstA64 sf:1;
};

DEFINE_OPCODE_A64(MOV_REG_1, 0b0101010000)
DEFINE_OPCODE_A64(MOV_REG_2, 0b00000011111)
DEFINE_STRUCT_A64(MOV_REG) {
    InstA64 rd:WideReg;
    InstA64 opcode2:11;
    InstA64 rm:WideReg;
    InstA64 opcode1:10;
    InstA64 sf:1;
};

DEFINE_OPCODE_A64(B_BL, 0b00101)
DEFINE_STRUCT_A64(B_BL) {
    InstA64 imm26:26;
    InstA64 opcode:5;
    InstA64 op:1;
};

DEFINE_OPCODE_A64(CBZ_CBNZ, 0b011010)
DEFINE_STRUCT_A64(CBZ_CBNZ) {
    InstA64 rt:WideReg;
    InstA64 imm19:19;
    InstA64 op:1;
    InstA64 opcode:6;
    InstA64 sf:1;
};

DEFINE_OPCODE_A64(B_COND, 0b01010100)
DEFINE_STRUCT_A64(B_COND) {
    InstA64 cond:4;
    InstA64 unkown_0:1;
    InstA64 imm19:19;
    InstA64 opcode:8;
};

DEFINE_OPCODE_A64(TBZ_TBNZ, 0b011011)
DEFINE_STRUCT_A64(TBZ_TBNZ) {
    InstA64 rt:WideReg;
    InstA64 imm14:14;
    InstA64 b40:5;
    InstA64 op:1;
    InstA64 opcode:6;
    InstA64 b5:1;
};

DEFINE_OPCODE_A64(LDR_LIT, 0b011000)
DEFINE_STRUCT_A64(LDR_LIT) {
    InstA64 rt:WideReg;
    InstA64 imm19:19;
    InstA64 opcode:6;
    InstA64 op:2;
};


DEFINE_OPCODE_A64(LDR_UIMM, 0b11100101)
DEFINE_STRUCT_A64(LDR_UIMM) {
    InstA64 rt:WideReg;
    InstA64 rn:WideReg;
    InstA64 imm12:12;
    InstA64 opcode:8;
    InstA64 size:2;
};

DEFINE_OPCODE_A64(LDR_IMM, 0b111000010)
DEFINE_STRUCT_A64(LDR_IMM) {
    InstA64 rt:WideReg;
    InstA64 rn:WideReg;
    InstA64 addrmode:2;
    InstA64 imm9:9;
    InstA64 opcode:9;
    InstA64 size:2;
};

DEFINE_OPCODE_A64(LDRSW_UIMM, 0b11100110)
struct STRUCT_A64(LDRSW_UIMM) : public STRUCT_A64(LDR_UIMM) {
};

DEFINE_OPCODE_A64(LDRSW_IMM, 0b111000100)
struct STRUCT_A64(LDRSW_IMM) : public STRUCT_A64(LDR_IMM) {
};

DEFINE_OPCODE_A64(BR_BLR_RET_1, 0b110101100)
DEFINE_OPCODE_A64(BR_BLR_RET_2, 0b11111000000)
DEFINE_OPCODE_A64(BR_BLR_RET_3, 0b00000)
DEFINE_STRUCT_A64(BR_BLR_RET) {
    InstA64 opcode3:5;
    InstA64 rn:WideReg;
    InstA64 opcode2:11;
    InstA64 op:2;
    InstA64 opcode1:9;

};

DEFINE_OPCODE_A64(STR_IMM, 0b111000000)
DEFINE_STRUCT_A64(STR_IMM) {
    InstA64 rt:WideReg;
    InstA64 rn:WideReg;
    InstA64 addrmode:2;
    InstA64 imm9:9;
    InstA64 opcode:9;
    InstA64 size:2;
};

DEFINE_OPCODE_A64(STR_UIMM, 0b11100100)
DEFINE_STRUCT_A64(STR_UIMM) {
    InstA64 rt:WideReg;
    InstA64 rn:WideReg;
    InstA64 imm12:12;
    InstA64 opcode:8;
    InstA64 size:2;
};

DEFINE_OPCODE_A64(SUB_EXT_REG_1, 0b1)
DEFINE_OPCODE_A64(SUB_EXT_REG_2, 0b01011001)
DEFINE_STRUCT_A64(SUB_EXT_REG) {
    InstA64 rd:WideReg;
    InstA64 rn:WideReg;
    InstA64 imm3:3;
    InstA64 option:3;
    InstA64 rm:WideReg;
    InstA64 opcode2:8;
    InstA64 S:1;
    InstA64 opcode1:1;
    InstA64 sf:1;
};

DEFINE_OPCODE_A64(EXCEPTION_GEN_1, 0b11010100)
DEFINE_OPCODE_A64(EXCEPTION_GEN_2, 0b000)
DEFINE_STRUCT_A64(EXCEPTION_GEN) {
    InstA64 ll:2;
    InstA64 opcode2:3;
    InstA64 imm16:16;
    InstA64 op:3;
    InstA64 opcode1:8;
};

struct STRUCT_A64(SVC) : STRUCT_A64(EXCEPTION_GEN) {
};

DEFINE_OPCODE_A64(STP_LDP, 0b10100)
DEFINE_STRUCT_A64(STP_LDP) {
    InstA64 rt:WideReg;
    InstA64 rn:WideReg;
    InstA64 rt2:WideReg;
    InstA64 imm7:7;
    InstA64 op:1;
    InstA64 addrmode:2;
    InstA64 opcode:5;
    InstA64 size:2;
};

DEFINE_OPCODE_A64(ADD_SUB_IMM, 0b10001)
DEFINE_STRUCT_A64(ADD_SUB_IMM) {
    InstA64 rd:WideReg;
    InstA64 rn:WideReg;
    InstA64 imm12:12;
    InstA64 shift:2;
    InstA64 opcode:5;
    InstA64 S:1;
    InstA64 op:1;
    InstA64 sf:1;
};

DEFINE_OPCODE_A64(MSR_MRS, 0b1101010100)
DEFINE_STRUCT_A64(MSR_MRS) {
    InstA64 rt:WideReg;
    InstA64 sysreg:16;
    InstA64 op:1;
    InstA64 opcode:10;
};