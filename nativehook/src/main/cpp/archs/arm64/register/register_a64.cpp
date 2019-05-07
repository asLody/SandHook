//
// Created by swift on 2019/5/8.
//

#include "register_a64.h"
#include "register_list_a64.h"

using namespace SandHook::Asm;

RegisterA64::RegisterA64() {}

RegisterA64::RegisterA64(U8 code) : Register(code) {}



XRegister* XRegister::registers[AARCH64_REGISTER_COUNT] = {
#define DEFINE_REGISTERS(N) \
        &X##N,
        AARCH64_REGISTER_CODE_LIST(DEFINE_REGISTERS)
#undef DEFINE_REGISTERS
};

XRegister::XRegister(U8 code) : RegisterA64(code) {}

XRegister::XRegister() {}

U8 XRegister::getWide() {
    return Reg64Bit;
}



WRegister* WRegister::registers[AARCH64_REGISTER_COUNT] = {
#define DEFINE_REGISTERS(N) \
        &W##N,
        AARCH64_REGISTER_CODE_LIST(DEFINE_REGISTERS)
#undef DEFINE_REGISTERS
};

WRegister::WRegister(U8 code) : RegisterA64(code) {}

WRegister::WRegister() {}

U8 WRegister::getWide() {
    return Reg32Bit;
}
