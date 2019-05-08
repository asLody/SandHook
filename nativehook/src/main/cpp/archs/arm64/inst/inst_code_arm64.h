//
// Created by swift on 2019/5/8.
//

#ifndef SANDHOOK_NH_INST_CODE_ARM64_H
#define SANDHOOK_NH_INST_CODE_ARM64_H

#include "inst_struct_aarch64.h"

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

// Unconditional branch.
enum UnconditionalBranchOp {
    UnconditionalBranchFixed = 0x14000000,
    UnconditionalBranchFMask = 0x7C000000,
    UnconditionalBranchMask  = 0xFC000000,
    B                        = UnconditionalBranchFixed | 0x00000000,
    BL                       = UnconditionalBranchFixed | 0x80000000
};

#endif //SANDHOOK_NH_INST_CODE_ARM64_H
