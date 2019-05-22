//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_REGISTER_H
#define SANDHOOK_REGISTER_H

#include "../includes/base.h"

namespace SandHook {
    namespace Asm {


        enum RegVariant {
            Reg32Bit = 32,
            Reg64Bit = 64,
            Reg128Bit = 128
        };

        template <typename Data>
        class Register {
        public:

            Register() {}

            Register(U8 code) : code(code) {}

            inline U8 getCode() {
                return code;
            }

            virtual bool isFP() {
                return false;
            };

            virtual U8 getWide() {
                return 0;
            };

            inline U8 getWideInBytes() {
                return static_cast<U8>(getWide() / BITS_OF_BYTE);
            };

            inline bool is32Bit() {
                return getWide() == Reg32Bit;
            }

            inline bool is64Bit() {
                return getWide() == Reg64Bit;
            }

            inline bool is128Bit() {
                return getWide() == Reg128Bit;
            }

            virtual bool is(const Register &rhs) {
                return rhs.code == code;
            }

            bool operator==(const Register &rhs) const {
                return code == rhs.code;
            }

            bool operator!=(const Register &rhs) const {
                return !(rhs == *this);
            }

            virtual void setData(Data data) {
                this->data = data;
            }

            virtual Data& getData() {
                return data;
            }

        private:
            U8 code;
            Data data;
        };
    }
}

#endif //SANDHOOK_REGISTER_H
