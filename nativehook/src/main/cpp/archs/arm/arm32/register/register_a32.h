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

            RegisterA32();

            RegisterA32(U8 code);

            U8 getWide() override;

            static RegisterA32* get(U8 code) {
                return registers[code];
            }

        private:
            static RegisterA32* registers[];
        };
    }
}

#endif //SANDHOOK_NH_REGISTER_A32_H
