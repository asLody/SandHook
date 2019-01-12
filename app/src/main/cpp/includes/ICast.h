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

    private:
        T origin;
    };

    template <typename PType, typename MType>
    class IMember {
    public:

        IMember(PType p) {
            this->parent = p;
        }

        IMember(PType p, Size size) {
            this->parent = p;
            this->parentSize = size;
        }

        Size getOffset() {
            if (offset < 0) {
                offset = calOffset();
            }
            return offset;
        }

        Size getParentSize() {
            if (parentSize < 0) {
                offset = sizeof(PType);
            }
            return offset;
        }

        MType get() {
            return reinterpret_cast<MType>(&parent + getOffset());
        };

        void set(MType t) {
            memcpy(&parent + getOffset(), &t, sizeof(MType));
        };

    private:
        PType parent;
        Size offset = -1;
    protected:
        virtual Size calOffset();
        Size parentSize = - 1;

    };

}