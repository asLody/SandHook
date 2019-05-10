//
// Created by swift on 2019/5/7.
//

#ifndef SANDHOOK_NH_ASSEMBLER_H
#define SANDHOOK_NH_ASSEMBLER_H

#include "label.h"
#include "instruction.h"
#include "data.h"

namespace SandHook {

    using namespace Asm;

    namespace Assembler {

        class Assembler {

        };

        class CodeBuffer {
        public:
            void* getBuffer(Unit<Base>* unit);
        };

        class CodeContainer {
        public:
            void append(Unit<Base>* unit);
            void commit();
        private:
            void* startPc = nullptr;
            Addr curPc = 0;
            Addr maxSize = 0;
            std::list<Unit<Base>*> units = std::list<Unit<Base>*>();
            std::list<Label*> labels = std::list<Label*>();
            CodeBuffer* codeBuffer = nullptr;
        };


    }

}

#endif //SANDHOOK_NH_ASSEMBLER_H
