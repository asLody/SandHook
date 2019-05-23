//
// Created by swift on 2019/5/12.
//

#ifndef SANDHOOK_NH_REGISTER_A32_H
#define SANDHOOK_NH_REGISTER_A32_H

#include "base.h"
#include "register.h"


namespace SandHook {

    namespace Asm {

        union A32RegisterStruct {
            struct {
                U8 b3;
                U8 b2;
                U8 b1;
                U8 b0;
            } u8;
            struct {
                U16 h;
                U16 l;
            } u16;
        };

        class RegisterA32 : public Register<A32RegisterStruct> {
        public:

            RegisterA32();

            RegisterA32(U8 code);

            U8 getWide() override;

            virtual bool isUnkonw () {
                return getCode() == 38;
            }

            static RegisterA32* get(U8 code) {
                return registers[code];
            }

        private:
            static RegisterA32* registers[];
        };
    }
}

#endif //SANDHOOK_NH_REGISTER_A32_H
