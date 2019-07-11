//
// Created by swift on 2019/5/10.
//

#pragma once

#include "unit.h"

#define DATA(BITS) STRUCT_DATA_##BITS

#define DEFINE_DATA(BITS) \
struct DATA(BITS) : public Base { \
    U##BITS raw_;    \
public: \
    DATA(BITS)(U##BITS r) {    \
        raw_ = r;    \
    }   \
};

namespace SandHook {
    namespace Asm {

        template <typename D>
        class Data : public Unit<D> {
        public:

            Data(D raw) : backup_(raw) {}

            INLINE UnitTypeDef UnitType() override {
                return UnitTypeDef::UnitData;
            };

            void Move(D *dest) override {
                *dest = backup_;
            }

        private:
            D backup_;
        };


        DEFINE_DATA(16)
        class Data16 : public Data<DATA(16)> {
        public:
            Data16(U16 raw) : Data(DATA(16)(raw)) {}
        };

        DEFINE_DATA(32)
        class Data32 : public Data<DATA(32)> {
        public:
            Data32(U32 raw) : Data(DATA(32)(raw)) {}
        };

        DEFINE_DATA(64)
        class Data64 : public Data<DATA(64)> {
        public:
            Data64(U64 raw) : Data(DATA(64)(raw)) {}
        };

    }
}