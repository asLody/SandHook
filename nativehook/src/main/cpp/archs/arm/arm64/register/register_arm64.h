//
// Created by swift on 2019/5/8.
//
#pragma once

#include "register.h"

namespace SandHook {

    namespace Asm {

        union A64RegisterStruct {
            struct {
                U16 d3;
                U16 d2;
                U16 d1;
                U16 d0;
            } u16;
            struct {
                U32 h;
                U32 l;
            } u32;
        };

        class RegisterA64 : public Register<A64RegisterStruct> {
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

            U8 Wide() override;

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

            U8 Wide() override;

            static WRegister* get(U8 code) {
                return registers[code];
            }

        private:
            static WRegister* registers[];
        };



        class SystemRegister {
        public:

            struct SystemReg {
                U16 op2:3;
                U16 CRm:4;
                U16 CRn:4;
                U16 op1:3;
                U16 op0:2;
            };

            SystemRegister() {}

            SystemRegister(U16 value) : value(value) {}

            SystemRegister(U16 op0, U16 op1, U16 crn, U16 crm, U16 op2) {
                reg.op0 = op0;
                reg.op1 = op1;
                reg.CRn = crn;
                reg.CRm = crm;
                reg.op2 = op2;
                value = ForceCast<U16>(reg);
            }

            bool operator==(const SystemRegister &rhs) const {
                return value == rhs.value;
            }

            bool operator!=(const SystemRegister &rhs) const {
                return value != rhs.value;
            }

        public:
            U16 value;
            SystemReg reg;
        };


    }

}