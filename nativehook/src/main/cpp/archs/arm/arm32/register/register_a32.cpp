//
// Created by swift on 2019/5/12.
//

#include "register_a32.h"
#include "register_list_a32.h"

using namespace SandHook::RegistersA32;

SandHook::Asm::RegisterA32::RegisterA32(U8 code) : Register(code) {
}

SandHook::Asm::RegisterA32::RegisterA32() {

}

U8 SandHook::Asm::RegisterA32::getWide() {
    return 4;
}

RegisterA32* RegisterA32::registers[ARM32_REGISTER_COUNT] = {
#define DEFINE_REGISTERS_X(N) \
        &R##N,
        ARM32_REGISTER_CODE_LIST(DEFINE_REGISTERS_X)
#undef DEFINE_REGISTERS_X
};