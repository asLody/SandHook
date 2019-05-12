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
            virtual void* getBuffer(U32 size) = 0;
            virtual void resetLastBufferSize(U32 size){};
        };

        class CodeContainer {
        public:

            CodeContainer(CodeBuffer *codeBuffer);

            void setCodeBuffer(CodeBuffer *codeBuffer);

            //allow code relocate to get new pc first
            void allocBufferFirst(U32 size);
            void append(Unit<Base>* unit);
            void commit();

        public:
            //before commit is virtual address so = 0, after commit is real address
            Addr startPc = 0;
            Addr curPc = 0;
        private:
            Addr maxSize = 0;
            std::list<Unit<Base>*> units = std::list<Unit<Base>*>();
            std::list<Label*> labels = std::list<Label*>();
            CodeBuffer* codeBuffer = nullptr;
        };


    }

}

#endif //SANDHOOK_NH_ASSEMBLER_H
