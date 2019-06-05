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
            virtual bool visit(Unit<Base>* unit, void* pc) {
                delete unit;
                return false;
            };
        };

        class DefaultVisitor : public InstVisitor {
        public:
            DefaultVisitor(bool (*visitor)(Unit<Base> *, void *));

            bool visit(Unit<Base> *unit, void *pc) override;
        private:
            bool (*visitor)(Unit<Base>*, void*);
        };

        class InstDecoder {
        public:
            virtual void decode(void* codeStart, Addr codeLen, InstVisitor& visitor, bool onlyPcRelInst = false) = 0;
            inline void decode(void* codeStart, Addr codeLen, bool (*visitor)(Unit<Base>*, void*), bool onlyPcRelInst = false) {
                InstVisitor vis = DefaultVisitor(visitor);
                decode(codeStart, codeLen, vis, onlyPcRelInst);
            };
        };


        class Disassembler {
        public:
            static InstDecoder* get(Arch arch);
            static InstDecoder* get();
        };
    }
}

#endif //SANDHOOK_NH_DECODER_H
