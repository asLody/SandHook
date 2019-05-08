//
// Created by swift on 2019/5/5.
//

#ifndef SANDHOOK_NH_INST_AARCH64_H
#define SANDHOOK_NH_INST_AARCH64_H


#include "../../../asm/instruction.h"
#include "inst_code_arm64.h"

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

#define IMM_LO_W 2
#define IMM_HI_W 19
struct aarch64_adr_adrp {
    InstA64 op:1;
    InstA64 immlo:IMM_LO_W;
    InstA64 opcode:5;
    InstA64 immhi:IMM_HI_W;
    InstA64 rd:5;
};


#define MOV_WIDE_OPCODE 0b100101
struct aarch64_mov_wide {
    InstA64 sf:1;
    InstA64 opc:2;
    InstA64 opcode:6;
    InstA64 hw:2;
    InstA64 imm16:16;
    InstA64 rd:5;
};

#define B_BL_OPCODE 0b00101
struct aarch64_b_bl {
    InstA64 op:1;
    InstA64 opcode:5;
    InstA64 imm26:26;
};

#define CBZ_CBNZ_OPCODE 0b011010
struct aarch64_cbz_cbnz {
    InstA64 sf:1;
    InstA64 opcode:6;
    InstA64 op:1;
    InstA64 imm19:19;
    InstA64 rt:5;
};

#endif //SANDHOOK_NH_INST_AARCH64_H
