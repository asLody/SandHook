//
// Created by swift on 2019/5/8.
//

#include "register_arm64.h"
#include "register_list_arm64.h"

using namespace SandHook::Asm;
using namespace SandHook::RegistersA64;


RegisterA64::RegisterA64() {}

RegisterA64::RegisterA64(U8 code) : Register(code) {}



XRegister* XRegister::registers[AARCH64_REGISTER_COUNT] = {
#define DEFINE_REGISTERS_X(N) \
        &X##N,
        AARCH64_REGISTER_CODE_LIST(DEFINE_REGISTERS_X)
#undef DEFINE_REGISTERS_X
};

XRegister::XRegister(U8 code) : RegisterA64(code) {}

XRegister::XRegister() {}

U8 XRegister::Wide() {
    return Reg64Bit;
}



WRegister* WRegister::registers[AARCH64_REGISTER_COUNT] = {
#define DEFINE_REGISTERS_W(N) \
        &W##N,
        AARCH64_REGISTER_CODE_LIST(DEFINE_REGISTERS_W)
#undef DEFINE_REGISTERS_W
};

WRegister::WRegister(U8 code) : RegisterA64(code) {}

WRegister::WRegister() {}

U8 WRegister::Wide() {
    return Reg32Bit;
}
