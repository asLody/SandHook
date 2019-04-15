//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_REGISTER_H
#define SANDHOOK_REGISTER_H

#include "../includes/base.h"

namespace SandHook {
    namespace Asm {
        class Register {
        public:
            bool isFP() {
                return false;
            };

        private:
            U8 num;
            //byte
            U8 wide;
            U32 code;
            Arch arch;
            InstType instType;
        };
    }
}

#endif //SANDHOOK_REGISTER_H
