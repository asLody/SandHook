//
// Created by swift on 2019/5/8.
//

#ifndef SANDHOOK_NH_REGISTER_A64_H
#define SANDHOOK_NH_REGISTER_A64_H

#include "register.h"

namespace SandHook {

    namespace Asm {

        class RegisterA64 : public Register {
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

            enum SpecialRegisters {
                kFpRegCode = 29,
                kLinkRegCode = 30,
                kSpRegCode = 31,
                kZeroRegCode = 31,
                kSPRegInternalCode = 63
            };

            RegisterA64();
            RegisterA64(U8 code);

            inline bool isX() {
                return is64Bit();
            }

            inline bool isW() {
                return is32Bit();
            }
        };

        class XRegister : public RegisterA64 {
        public:
            XRegister();
            XRegister(U8 code);

            U8 getWide() override;

            static XRegister* get(U8 code) {
                return registers[code];
            }

        private:
            static XRegister* registers[];
        };

        class WRegister : public RegisterA64 {
        public:
            WRegister();
            WRegister(U8 code);

            U8 getWide() override;

            static WRegister* get(U8 code) {
                return registers[code];
            }

        private:
            static WRegister* registers[];
        };

    }

}

#endif //SANDHOOK_NH_REGISTER_A64_H
