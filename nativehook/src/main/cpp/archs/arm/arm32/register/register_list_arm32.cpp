//
// Created by swift on 2019/5/16.
//

#include "register_list_arm32.h"

namespace SandHook {
namespace RegistersA32 {

#define INIT_REGISTERS(N) \
  RegisterA32 R##N = RegisterA32(N);
    ARM32_REGISTER_CODE_LIST(INIT_REGISTERS)
#undef INIT_REGISTERS

    enum RegNum {
        kIPRegNum = 12, kSPRegNum = 13, kLRRegNum = 14, kPCRegNum = 15
    };

    RegisterA32 SP = *Reg(kSPRegNum);
    RegisterA32 IP = *Reg(kIPRegNum);
    RegisterA32 LR = *Reg(kLRRegNum);
    RegisterA32 PC = *Reg(kPCRegNum);
    RegisterA32 UnknowRegiser = RegisterA32(38);

}}