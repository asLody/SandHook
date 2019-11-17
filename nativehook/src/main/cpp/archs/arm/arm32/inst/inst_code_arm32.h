//
// Created by swift on 2019/5/16.
//

#pragma once

enum class InstCodeA32 : InstCode {

};

enum class InstCodeT16 : InstCode {
    UNKNOW,
    BASE_SASMC,
    DATA_PROC,
    SPDIABE,
    MISC,
    B,
    B_COND,
    BX_BLX,
    CBZ_CBNZ,
    LDR_LIT,
    ADD_IMM_RDN,
    //rn = pc
    ADR,
    ADD_REG,
    CMP_REG,
    CMP_REG_EXT,
    MOV_IMM,
    MOV_REG,
    POP,
    PUSH,
    ADD_REG_RDN
};

enum class InstCodeT32 : InstCode {
    UNKNOW,
    B32,
    LDR_LIT,
    LDR_IMM,
    LDR_UIMM,
    MOV_MOVT_IMM,
    SUB_IMM,
    HVC
};