//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_IMME_H
#define SANDHOOK_IMME_H

#include "../includes/base.h"

namespace SandHook {
    namespace Asm {

        template <typename ImmeType>
        class Imme {
        public:
            virtual ImmeType get() = 0;
            virtual void set(ImmeType imme) = 0;

            U8 size() {
                return immeSize;
            };

        private:
            //byte
            U8 immeSize;
            ImmeType value;
            Arch arch;
            InstType instType;
        };
    }
}

#endif //SANDHOOK_IMME_H
