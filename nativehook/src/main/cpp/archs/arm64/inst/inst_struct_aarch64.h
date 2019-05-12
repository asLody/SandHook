//
// Created by swift on 2019/5/5.
//

#ifndef SANDHOOK_NH_INST_AARCH64_H
#define SANDHOOK_NH_INST_AARCH64_H


#include "instruction.h"
#include "inst_code_arm64.h"

#define STRUCT_A64(X) A64_STRUCT_##X
#define OPCODE_A64(X) A64_OPCODE_##X
#define DEFINE_OPCODE(X, V) const U32 OPCODE_A64(X) = V;
#define DEFINE_STRUCT_A64(X) struct STRUCT_A64(X) : public Base

enum InstructionFields {

    // Registers.
    kRdShift  = 0,
    kRdBits   = 5,
    kRnShift  = 5,
    kRnBits   = 5,
    kRaShift  = 10,
    kRaBits   = 5,
    kRmShift  = 16,
    kRmBits   = 5,
    kRtShift  = 0,
    kRtBits   = 5,
    kRt2Shift = 10,
    kRt2Bits  = 5,
    kRsShift  = 16,
    kRsBits   = 5,

};

enum ImmBranchType {
    UnknownBranchType = 0,
    CondBranchType = 1,
    UncondBranchType = 2,
    CompareBranchType = 3,
    TestBranchType = 4
};

// Condition codes.
enum Condition {
    eq = 0,   // Z set            Equal.
    ne = 1,   // Z clear          Not equal.
    cs = 2,   // C set            Carry set.
    cc = 3,   // C clear          Carry clear.
    mi = 4,   // N set            Negative.
    pl = 5,   // N clear          Positive or zero.
    vs = 6,   // V set            Overflow.
    vc = 7,   // V clear          No overflow.
    hi = 8,   // C set, Z clear   Unsigned higher.
    ls = 9,   // C clear or Z set Unsigned lower or same.
    ge = 10,  // N == V           Greater or equal.
    lt = 11,  // N != V           Less than.
    gt = 12,  // Z clear, N == V  Greater than.
    le = 13,  // Z set or N != V  Less then or equal
    al = 14,  //                  Always.
    nv = 15,  // Behaves as always/al.

    // Aliases.
    hs = cs,  // C set            Unsigned higher or same.
    lo = cc   // C clear          Unsigned lower.
};

enum Shift {
    NO_SHIFT = -1,
    LSL = 0x0,
    LSR = 0x1,
    ASR = 0x2,
    ROR = 0x3,
    MSL = 0x4
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

//unknow inst
DEFINE_STRUCT_A64(UNKNOW) {
    InstA64 raw;
};

#define IMM_LO_W 2
#define IMM_HI_W 19
DEFINE_OPCODE(ADR_ADRP, 0b10000)
struct STRUCT_A64(ADR_ADRP) {
    InstA64 rd:WideReg;
    InstA64 immhi:IMM_HI_W;
    InstA64 opcode:5;
    InstA64 immlo:IMM_LO_W;
    InstA64 op:1;
};

DEFINE_OPCODE(MOV_WIDE, 0b100101)
DEFINE_STRUCT_A64(MOV_WIDE) {
    InstA64 rd:WideReg;
    InstA64 imm16:16;
    InstA64 hw:2;
    InstA64 opcode:6;
    InstA64 opc:2;
    InstA64 sf:1;
};

DEFINE_OPCODE(MOV_REG_1, 0b0101010000)
DEFINE_OPCODE(MOV_REG_2, 0b00000011111)
DEFINE_STRUCT_A64(MOV_REG) {
    InstA64 rd:WideReg;
    InstA64 opcode2:11;
    InstA64 rm:WideReg;
    InstA64 opcode1:10;
    InstA64 sf:1;
};

DEFINE_OPCODE(B_BL, 0b00101)
DEFINE_STRUCT_A64(B_BL) {
    InstA64 imm26:26;
    InstA64 opcode:5;
    InstA64 op:1;
};

DEFINE_OPCODE(CBZ_CBNZ, 0b011010)
DEFINE_STRUCT_A64(CBZ_CBNZ) {
    InstA64 rt:WideReg;
    InstA64 imm19:19;
    InstA64 op:1;
    InstA64 opcode:6;
    InstA64 sf:1;
};

DEFINE_OPCODE(B_COND, 0b01010100)
DEFINE_STRUCT_A64(B_COND) {
    InstA64 cond:4;
    InstA64 unkown_0:1;
    InstA64 imm19:19;
    InstA64 opcode:8;
};

DEFINE_OPCODE(TBZ_TBNZ, 0b011011)
DEFINE_STRUCT_A64(TBZ_TBNZ) {
    InstA64 rt:WideReg;
    InstA64 imm14:14;
    InstA64 b40:5;
    InstA64 op:1;
    InstA64 opcode:6;
    InstA64 b5:1;
};

DEFINE_OPCODE(LDR_LIT, 0b011000)
DEFINE_STRUCT_A64(LDR_LIT) {
    InstA64 rt:WideReg;
    InstA64 imm19:19;
    InstA64 opcode:6;
    InstA64 op:2;
};

DEFINE_OPCODE(BR_BLR_RET_1, 0b110101100)
DEFINE_OPCODE(BR_BLR_RET_2, 0b11111000000)
DEFINE_OPCODE(BR_BLR_RET_3, 0b00000)
DEFINE_STRUCT_A64(BR_BLR_RET) {
    InstA64 opcode3:5;
    InstA64 rn:WideReg;
    InstA64 opcode2:11;
    InstA64 op:2;
    InstA64 opcode1:9;

};

DEFINE_OPCODE(STR_IMM, 0b111000000)
DEFINE_STRUCT_A64(STR_IMM) {
    InstA64 rt:WideReg;
    InstA64 rn:WideReg;
    InstA64 addrmode:2;
    InstA64 imm9:9;
    InstA64 opcode:9;
    InstA64 size:2;
};

DEFINE_OPCODE(STR_UIMM, 0b11100100)
DEFINE_STRUCT_A64(STR_UIMM) {
    InstA64 rt:WideReg;
    InstA64 rn:WideReg;
    InstA64 imm12:12;
    InstA64 opcode:8;
    InstA64 size:2;
};

DEFINE_OPCODE(SUB_EXT_REG_1, 0b1)
DEFINE_OPCODE(SUB_EXT_REG_2, 0b01011001)
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

DEFINE_OPCODE(EXCEPTION_GEN_1, 0b11010100)
DEFINE_OPCODE(EXCEPTION_GEN_2, 0b11100100)
DEFINE_STRUCT_A64(EXCEPTION_GEN) {
    InstA64 ll:2;
    InstA64 opcode2:3;
    InstA64 imm16:16;
    InstA64 op:3;
    InstA64 opcode1:8;
};


#endif //SANDHOOK_NH_INST_AARCH64_H
