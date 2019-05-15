//
// Created by swift on 2019/5/12.
//

#ifndef SANDHOOK_NH_REGISTER_A32_H
#define SANDHOOK_NH_REGISTER_A32_H

#include "base.h"
#include "register.h"


namespace SandHook {

    namespace Asm {

        class RegisterA32 : public Register {
        public:

            enum RegisterType {
                // The kInvalid value is used to detect uninitialized static instances,
                // which are always zero-initialized before any constructors are called.
                kInvalid = 0,
                kRegister,
                kVRegister,
                kFPRegister = kVRegister,
                kNoRegister
            };

            RegisterA32();

            RegisterA32(U8 code);
        };
    }
}

#endif //SANDHOOK_NH_REGISTER_A32_H
