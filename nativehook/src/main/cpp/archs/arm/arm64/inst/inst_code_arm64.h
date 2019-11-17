//
// Created by swift on 2019/5/8.
//

#pragma once

#include "inst_struct_aarch64.h"

enum class InstCodeA64 : InstCode {
    UNKNOW,
    MOV_WIDE,
    MOV_REG,
    ADR_ADRP,
    LDR_LIT,
    LDR_UIMM,
    LDR_IMM,
    LDRSW_UIMM,
    LDRSW_IMM,
    STR_IMM,
    STR_UIMM,
    B_BL,
    B_COND,
    BR_BLR_RET,
    CBZ_CBNZ,
    TBZ_TBNZ,
    SUB_EXT_REG,
    EXCEPTION_GEN,
    SVC,
    STP_LDP,
    ADD_SUB_IMM,
    MSR_MRS
};