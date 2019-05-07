//
// Created by swift on 2019/5/8.
//

#ifndef SANDHOOK_NH_REGISTER_A64_H
#define SANDHOOK_NH_REGISTER_A64_H

#include "register.h"

// clang-format off

#define AARCH64_REGISTER_COUNT 32

#define AARCH64_REGISTER_CODE_LIST(R)                                          \
  R(0)  R(1)  R(2)  R(3)  R(4)  R(5)  R(6)  R(7)                               \
  R(8)  R(9)  R(10) R(11) R(12) R(13) R(14) R(15)                              \
  R(16) R(17) R(18) R(19) R(20) R(21) R(22) R(23)                              \
  R(24) R(25) R(26) R(27) R(28) R(29) R(30) R(31)


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
