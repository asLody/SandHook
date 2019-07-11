//
// Created by SwiftGan on 2019/4/15.
//

#pragma once

#include "base.h"

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

            Register(U8 code) : code_(code) {}

            inline U8 Code() {
                return code_;
            }

            virtual bool IsFP() {
                return false;
            };

            virtual U8 Wide() {
                return 0;
            };

            INLINE U8 WideInBytes() {
                return static_cast<U8>(Wide() / BITS_OF_BYTE);
            };

            INLINE bool is32Bit() {
                return Wide() == Reg32Bit;
            }

            INLINE bool is64Bit() {
                return Wide() == Reg64Bit;
            }

            INLINE bool is128Bit() {
                return Wide() == Reg128Bit;
            }

            bool operator==(const Register &rhs) const {
                return code_ == rhs.code_;
            }

            bool operator!=(const Register &rhs) const {
                return code_ != rhs.code_;
            }

            virtual void SetData(Data data) {
                data_ = data;
            }

            virtual Data& GetData() {
                return data_;
            }

        private:
            U8 code_;
            Data data_;
        };
    }
}