//
// Created by swift on 2019/5/8.
//

#ifndef SANDHOOK_NH_REGISTER_LIST_A64_H
#define SANDHOOK_NH_REGISTER_LIST_A64_H

#include "register_a64.h"
#include "../../../includes/base.h"

using namespace SandHook::Asm;

#define DEFINE_REGISTERS(N) \
  WRegister W##N(N);  \
  XRegister X##N(N);
AARCH64_REGISTER_CODE_LIST(DEFINE_REGISTERS)
#undef DEFINE_REGISTERS

WRegister WSP(RegisterA64::kSPRegInternalCode);
XRegister SP(RegisterA64::kSPRegInternalCode);
XRegister IP0 = X16;
XRegister IP1 = X17;
XRegister LR = X30;
//zero reg
XRegister XZR = X31;
WRegister WZR = W31;

#endif //SANDHOOK_NH_REGISTER_LIST_A64_H
