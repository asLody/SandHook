//
// Created by swift on 2019/5/6.
//

#ifndef SANDHOOK_NH_DECODER_H
#define SANDHOOK_NH_DECODER_H

#include "../includes/base.h"
#include "../asm/instruction.h"

namespace SandHook {
    namespace Decoder {

        using namespace Asm;

        class InstVisitor {
        public:
            virtual bool visit(Instruction<void>* inst, ADDR offset, ADDR length) = 0;
        };

        class InstDecoder {
        public:
            virtual void decode(void* codeStart, ADDR codeLen, InstVisitor* visitor) = 0;
        };


        class Decoder {
            static InstDecoder* get(Arch arch);
        };
    }
}

#endif //SANDHOOK_NH_DECODER_H