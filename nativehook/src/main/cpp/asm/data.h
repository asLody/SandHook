//
// Created by swift on 2019/5/10.
//

#ifndef SANDHOOK_NH_DATA_H
#define SANDHOOK_NH_DATA_H

#include "unit.h"

namespace SandHook {
    namespace Asm {

        template <typename DType>
        class Data : public Unit<DType> {
        public:
            Data(DType raw) : Unit<DType>(raw) {}
            inline UnitType unitType() override {
                return UnitType::Data;
            };
        };

        class Data16 : public Data<U16> {
        public:
            Data16(U16 raw) : Data(raw) {}
        };

        class Data32 : public Data<U32> {
        public:
            Data32(U32 raw) : Data(raw) {}
        };

        class Data64 : public Data<U64> {
        public:
            Data64(U64 raw) : Data(raw) {}
        };

    }
}

#endif //SANDHOOK_NH_DATA_H
