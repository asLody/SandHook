//
// Created by swift on 2019/5/8.
//

#pragma once

#include "register_arm32.h"

using namespace SandHook::Asm;

    // clang-format off
#define ARM32_REGISTER_COUNT 16

#define ARM32_REGISTER_CODE_LIST(R)                                          \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)


namespace SandHook {
namespace RegistersA32 {
#define DEFINE_REGISTERS(N) \
  extern RegisterA32 R##N;
    ARM32_REGISTER_CODE_LIST(DEFINE_REGISTERS)
#undef DEFINE_REGISTERS

    extern RegisterA32 SP;
    extern RegisterA32 IP;
    extern RegisterA32 LR;
    extern RegisterA32 PC;
    extern RegisterA32 UnknowRegiser;
}}
#define Reg(N) RegisterA32::get(N)