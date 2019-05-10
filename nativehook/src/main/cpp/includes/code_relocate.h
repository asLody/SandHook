//
// Created by swift on 2019/5/10.
//

#ifndef SANDHOOK_NH_CODE_RELOCATE_H
#define SANDHOOK_NH_CODE_RELOCATE_H

#include "exception.h"
#include "instruction.h"

namespace SandHook {
    namespace Asm {

        template <typename Raw>
        class CodeRelocateCallback {
        public:
            virtual bool result(Unit<Raw>* unit, bool end) throw(ErrorCodeException) = 0;
        };

        template <typename Raw>
        class CodeRelocate {
            virtual bool relocate(Addr toPc, CodeRelocateCallback<Raw>& callback) throw(ErrorCodeException) = 0;
        };

    }
}

#endif //SANDHOOK_NH_CODE_RELOCATE_H
