//
// Created by swift on 2019/5/9.
//

#include "register_list_a64.h"

#define INIT_REGISTERS(N) \
    WRegister W##N = WRegister(N); \
    XRegister X##N = XRegister(N);
AARCH64_REGISTER_CODE_LIST(INIT_REGISTERS)
#undef INIT_REGISTERS

WRegister WSP = WRegister(RegisterA64::kSPRegInternalCode);
XRegister SP = XRegister(RegisterA64::kSPRegInternalCode);
XRegister IP0 = X16;
XRegister IP1 = X17;
XRegister LR = X30;
//zero reg
XRegister XZR = X31;
WRegister WZR = W31;
RegisterA64 UnknowRegiser = RegisterA64(38);