//
// Created by swift on 2019/5/8.
//

#pragma once

#include "register_arm64.h"
#include "base.h"

using namespace SandHook::Asm;

// clang-format off
#define AARCH64_REGISTER_COUNT 32

#define AARCH64_REGISTER_CODE_LIST(R)                                          \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)                              \
  R(16) R(17) R(18) R(19) R(20) R(21) R(22) R(23)                              \
  R(24) R(25) R(26) R(27) R(28) R(29) R(30) R(31)


namespace SandHook {
    namespace RegistersA64 {

#define DEFINE_REGISTERS(N) \
  extern WRegister W##N;  \
  extern XRegister X##N;
        AARCH64_REGISTER_CODE_LIST(DEFINE_REGISTERS)
#undef DEFINE_REGISTERS

        extern WRegister WSP;
        extern XRegister SP;
        extern XRegister IP0;
        extern XRegister IP1;
        extern XRegister LR;
//zero reg_
        extern XRegister XZR;
        extern WRegister WZR;
        extern RegisterA64 UnknowRegiser;


        //cmp....
        inline RegisterA64 *zeroRegFor(RegisterA64& registerA64) {
            if (registerA64.isX()) {
                return &XZR;
            } else {
                return &WZR;
            }
        }

        // System/special register names.
        // This information is not encoded as one field but as the concatenation of
        // multiple fields (Op0<0>, Op1, Crn, Crm, Op2).
        extern SystemRegister NZCV;
        extern SystemRegister FPCR;
    }
}

#define XReg(N) XRegister::get(N)
#define WReg(N) WRegister::get(N)