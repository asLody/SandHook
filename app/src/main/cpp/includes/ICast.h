//
// Created by 甘尧 on 2019/1/12.
//

#include <stdint.h>
#include <string.h>
#include "arch.h"

#ifndef SANDHOOK_ICAST_H
#define SANDHOOK_ICAST_H

#endif //SANDHOOK_ICAST_H


namespace SandHook {

    template <typename T>
    class ICast {
    public:
        ICast(T t) {
            this->origin = t;
        };

        virtual Size getSize() { return sizeof(T); };

    private:
        T origin;
    };

    template <typename PType, typename MType>
    class IMember {
    public:

        virtual void init(PType p, Size size) {
            this->parentSize = size;
            offset = calOffset(p);
        }

        virtual Size getOffset() {
            return offset;
        }

        virtual Size getParentSize() {
            return parentSize;
        }

        virtual MType get(PType p) {
            return reinterpret_cast<MType>(&p + getOffset());
        };

        virtual void set(PType p, MType t) {
            memcpy(&p + getOffset(), &t, sizeof(MType));
        };

    private:
        Size offset = 0;
    protected:
        Size parentSize = 0;
        virtual Size calOffset(PType p) = 0;

    };

}