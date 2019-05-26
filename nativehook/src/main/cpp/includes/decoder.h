//
// Created by swift on 2019/5/6.
//

#ifndef SANDHOOK_NH_DECODER_H
#define SANDHOOK_NH_DECODER_H

#include "base.h"
#include "instruction.h"

namespace SandHook {
    namespace Decoder {

        using namespace Asm;

        class InstVisitor {
        public:
            //need free unit
            virtual bool visit(Unit<Base>* unit, void* pc) = 0;
        };

        class InstDecoder {
        public:
            virtual void decode(void* codeStart, Addr codeLen, InstVisitor& visitor, bool onlyPcRelInst = false) = 0;
        };


        class Disassembler {
        public:
            static InstDecoder* get(Arch arch);
            static InstDecoder* get();
        };
    }
}

#endif //SANDHOOK_NH_DECODER_H
