//
// Created by 甘尧 on 2019/1/12.
//

#include <stdint.h>
#include <string.h>
#include "arch.h"
#include "utils.h"

#define ANDROID_L 21
#define ANDROID_L2 22
#define ANDROID_M 23
#define ANDROID_N 24
#define ANDROID_N2 25
#define ANDROID_O 26
#define ANDROID_O2 27
#define ANDROID_P 28

#ifndef SANDHOOK_ICAST_H
#define SANDHOOK_ICAST_H

namespace SandHook {

    static int SDK_INT = 0;

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
            if (offset > parentSize)
                return NULL;
            return *reinterpret_cast<MType*>(&p + getOffset());
        };

        virtual void set(PType p, MType t) {
            if (offset > parentSize)
                return;
            memcpy(&p + getOffset(), &t, sizeof(MType));
        };

    private:
        Size offset = 0;
    protected:
        Size parentSize = 0;
        virtual Size calOffset(PType p) = 0;

    };

}

#endif //SANDHOOK_ICAST_H