//
// Created by swift on 2019/5/8.
//

#ifndef SANDHOOK_NH_INST_CODE_ARM64_H
#define SANDHOOK_NH_INST_CODE_ARM64_H

#include "inst_struct_aarch64.h"

enum InstCodeA64 {
    UNKNOW,
    MOV_WIDE,
    MOV_REG,
    ADR_ADRP,
    LDR_LIT,
    STR_IMM,
    STR_UIMM,
    B_BL,
    B_COND,
    BR_BLR_RET,
    CBZ_CBNZ,
    TBZ_TBNZ,
    SUB_EXT_REG
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

// Unconditional branch.
enum UnconditionalBranchOp {
    UnconditionalBranchFixed = 0x14000000,
    UnconditionalBranchFMask = 0x7C000000,
    UnconditionalBranchMask  = 0xFC000000,
    B                        = UnconditionalBranchFixed | 0x00000000,
    BL                       = UnconditionalBranchFixed | 0x80000000
};

// Compare and branch.
enum CompareBranchOp {
    CompareBranchFixed = 0x34000000,
    CompareBranchFMask = 0x7E000000,
    CompareBranchMask  = 0xFF000000,
    CBZ_w              = CompareBranchFixed | 0x00000000,
    CBZ_x              = CompareBranchFixed | 0x80000000,
    CBZ                = CBZ_w,
    CBNZ_w             = CompareBranchFixed | 0x01000000,
    CBNZ_x             = CompareBranchFixed | 0x81000000,
    CBNZ               = CBNZ_w
};

// Conditional branch.
enum ConditionalBranchOp {
    ConditionalBranchFixed = 0x54000000,
    ConditionalBranchFMask = 0xFE000000,
    ConditionalBranchMask  = 0xFF000010,
    B_cond                 = ConditionalBranchFixed | 0x00000000
};

// Test and branch.
enum TestBranchOp {
    TestBranchFixed = 0x36000000,
    TestBranchFMask = 0x7E000000,
    TestBranchMask  = 0x7F000000,
    TBZ             = TestBranchFixed | 0x00000000,
    TBNZ            = TestBranchFixed | 0x01000000
};

// Load literal.
enum LoadLiteralOp {
    LoadLiteralFixed = 0x18000000,
    LoadLiteralFMask = 0x3B000000,
    LoadLiteralMask  = 0xFF000000,
    LDR_w_lit        = LoadLiteralFixed | 0x00000000,
    LDR_x_lit        = LoadLiteralFixed | 0x40000000,
    LDRSW_x_lit      = LoadLiteralFixed | 0x80000000,
    PRFM_lit         = LoadLiteralFixed | 0xC0000000,
    LDR_s_lit        = LoadLiteralFixed | 0x04000000,
    LDR_d_lit        = LoadLiteralFixed | 0x44000000,
    LDR_q_lit        = LoadLiteralFixed | 0x84000000
};


#define LOAD_STORE_OP_LIST(V)     \
  V(ST, RB, w,  0x00000000),  \
  V(ST, RH, w,  0x40000000),  \
  V(ST, R, w,   0x80000000),  \
  V(ST, R, x,   0xC0000000),  \
  V(LD, RB, w,  0x00400000),  \
  V(LD, RH, w,  0x40400000),  \
  V(LD, R, w,   0x80400000),  \
  V(LD, R, x,   0xC0400000),  \
  V(LD, RSB, x, 0x00800000),  \
  V(LD, RSH, x, 0x40800000),  \
  V(LD, RSW, x, 0x80800000),  \
  V(LD, RSB, w, 0x00C00000),  \
  V(LD, RSH, w, 0x40C00000),  \
  V(ST, R, b,   0x04000000),  \
  V(ST, R, h,   0x44000000),  \
  V(ST, R, s,   0x84000000),  \
  V(ST, R, d,   0xC4000000),  \
  V(ST, R, q,   0x04800000),  \
  V(LD, R, b,   0x04400000),  \
  V(LD, R, h,   0x44400000),  \
  V(LD, R, s,   0x84400000),  \
  V(LD, R, d,   0xC4400000),  \
  V(LD, R, q,   0x04C00000)

// Load/store (post, pre, offset and unsigned.)
enum LoadStoreOp {
    LoadStoreMask = 0xC4C00000,
    LoadStoreVMask = 0x04000000,
#define LOAD_STORE(A, B, C, D)  \
  A##B##_##C = D
    LOAD_STORE_OP_LIST(LOAD_STORE),
#undef LOAD_STORE
    PRFM = 0xC0800000
};

#endif //SANDHOOK_NH_INST_CODE_ARM64_H
