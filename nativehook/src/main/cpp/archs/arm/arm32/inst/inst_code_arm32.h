//
// Created by swift on 2019/5/16.
//

#ifndef SANDHOOK_INST_CODE_ARM32_H
#define SANDHOOK_INST_CODE_ARM32_H

enum class InstCodeA32 {

};

enum class InstCodeT16 {
    BASE_SASMC,
    DATA_PROC,
    SPDIABE,
    MISC,
    B,
    B_COND,
    BX_BLX,
    CBZ_CBNZ,
    LDR_LIT,
    //when rd = rn
    ADD_IMM_RDN,
    //rn = pc
    ADR,
    CMP_REG,
    MOV_REG
};

enum class InstCodeT32 {
    B32,
};

#endif //SANDHOOK_INST_CODE_ARM32_H
