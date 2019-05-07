//
// Created by swift on 2019/5/5.
//

#ifndef SANDHOOK_NH_INST_AARCH64_H
#define SANDHOOK_NH_INST_AARCH64_H


#include "../../../asm/instruction.h"

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


// Generic fields.
enum GenericInstrField {
    SixtyFourBits        = 0x80000000,
    ThirtyTwoBits        = 0x00000000,

    FPTypeMask           = 0x00C00000,
    FP16                 = 0x00C00000,
    FP32                 = 0x00000000,
    FP64                 = 0x00400000
};


enum PCRelAddressingOp {
    PCRelAddressingFixed = 0x10000000,
    PCRelAddressingFMask = 0x1F000000,
    PCRelAddressingMask  = 0x9F000000,
    ADR                  = PCRelAddressingFixed | 0x00000000,
    ADRP                 = PCRelAddressingFixed | 0x80000000
};


// Move wide immediate.
enum MoveWideImmediateOp {
    MoveWideImmediateFixed = 0x12800000,
    MoveWideImmediateFMask = 0x1F800000,
    MoveWideImmediateMask  = 0xFF800000,
    MOVN                   = 0x00000000,
    MOVZ                   = 0x40000000,
    MOVK                   = 0x60000000,
    MOVN_w                 = MoveWideImmediateFixed | MOVN,
    MOVN_x                 = MoveWideImmediateFixed | MOVN | SixtyFourBits,
    MOVZ_w                 = MoveWideImmediateFixed | MOVZ,
    MOVZ_x                 = MoveWideImmediateFixed | MOVZ | SixtyFourBits,
    MOVK_w                 = MoveWideImmediateFixed | MOVK,
    MOVK_x                 = MoveWideImmediateFixed | MOVK | SixtyFourBits
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
struct aarch64_pcrel_insts {
    InstA64 op:1;
    InstA64 immlo:IMM_LO_W;
    InstA64 opcode:5;
    InstA64 immhi:IMM_HI_W;
    InstA64 rd:5;
};

struct aarch64_adr_adrp : public aarch64_pcrel_insts {};


#define MOV_WIDE_OPCODE 0b100101
struct aarch64_mov_wide {
    InstA64 sf:1;
    InstA64 opc:2;
    InstA64 opcode:6;
    InstA64 hw:2;
    InstA64 imm16:16;
    InstA64 rd:5;
};

#endif //SANDHOOK_NH_INST_AARCH64_H
