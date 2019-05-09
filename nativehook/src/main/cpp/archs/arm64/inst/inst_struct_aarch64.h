//
// Created by swift on 2019/5/5.
//

#ifndef SANDHOOK_NH_INST_AARCH64_H
#define SANDHOOK_NH_INST_AARCH64_H


#include "../../../asm/instruction.h"
#include "inst_code_arm64.h"

#define STRUCT_A64(X) A64_STRUCT_##X
#define OPCODE_A64(X) A64_OPCODE_##X
#define DEFINE_OPCODE(X, V) const U32 OPCODE_A64(X) = V;

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

#define IMM_LO_W 2
#define IMM_HI_W 19
DEFINE_OPCODE(ADR_ADRP, 0b10000)
struct STRUCT_A64(ADR_ADRP) {
    InstA64 op:1;
    InstA64 immlo:IMM_LO_W;
    InstA64 opcode:5;
    InstA64 immhi:IMM_HI_W;
    InstA64 rd:5;
};


DEFINE_OPCODE(MOV_WIDE, 0b100101)
struct STRUCT_A64(MOV_WIDE) {
    InstA64 sf:1;
    InstA64 opc:2;
    InstA64 opcode:6;
    InstA64 hw:2;
    InstA64 imm16:16;
    InstA64 rd:5;
};

DEFINE_OPCODE(B_BL, 0b00101)
struct STRUCT_A64(B_BL) {
    InstA64 op:1;
    InstA64 opcode:5;
    InstA64 imm26:26;
};

DEFINE_OPCODE(CBZ_CBNZ, 0b011010)
struct STRUCT_A64(CBZ_CBNZ) {
    InstA64 sf:1;
    InstA64 opcode:6;
    InstA64 op:1;
    InstA64 imm19:19;
    InstA64 rt:5;
};

DEFINE_OPCODE(B_COND, 0b01010100)
struct STRUCT_A64(B_COND) {
    InstA64 opcode:8;
    InstA64 imm19:19;
    InstA64 unkown:1;
    InstA64 cond:4;
};


DEFINE_OPCODE(TBZ_TBNZ, 0b011011)
struct STRUCT_A64(TBZ_TBNZ) {
    InstA64 b5:1;
    InstA64 opcode:6;
    InstA64 op:1;
    InstA64 b40:5;
    InstA64 imm14:14;
    InstA64 rt:5;
};



#endif //SANDHOOK_NH_INST_AARCH64_H
