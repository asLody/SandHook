//
// Created by swift on 2019/5/12.
//

#pragma once

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

            U8 Wide() override;

            virtual bool isUnkonw () {
                return Code() == 38;
            }

            static RegisterA32* get(U8 code) {
                return registers[code];
            }

        private:
            static RegisterA32* registers[];
        };
    }
}