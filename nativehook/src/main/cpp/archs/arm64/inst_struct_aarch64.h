//
// Created by swift on 2019/5/5.
//

#ifndef SANDHOOK_NH_INST_AARCH64_H
#define SANDHOOK_NH_INST_AARCH64_H


#include "../../asm/instruction.h"


enum PCRelAddressingOp {
    PCRelAddressingFixed = 0x10000000,
    PCRelAddressingFMask = 0x1F000000,
    PCRelAddressingMask  = 0x9F000000,
    ADR                  = PCRelAddressingFixed | 0x00000000,
    ADRP                 = PCRelAddressingFixed | 0x80000000
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

#endif //SANDHOOK_NH_INST_AARCH64_H
