//
// Created by swift on 2019/5/6.
//

#pragma once

#include "base.h"
#include "instruction.h"

namespace SandHook {
    namespace Decoder {

        using namespace Asm;

        class InstVisitor {
        public:
            //need free unit
            virtual bool Visit(Unit<Base> *unit, void *pc) {
                delete unit;
                return false;
            };
        };

        class DefaultVisitor : public InstVisitor {
        public:
            DefaultVisitor(bool (*visitor)(BaseUnit *, void *));

            bool Visit(BaseUnit *unit, void *pc) override;
        private:
            bool (*visitor)(BaseUnit*, void*);
        };

        class InstDecoder {
        public:
            virtual void Disassembler(void *codeStart, Addr codeLen, InstVisitor &visitor,
                                      bool onlyPcRelInst = false) = 0;
            inline void Disassembler(void* codeStart, Addr codeLen, bool (*visitor)(Unit<Base>*, void*), bool onlyPcRelInst = false) {
                InstVisitor vis = DefaultVisitor(visitor);
                Disassembler(codeStart, codeLen, vis, onlyPcRelInst);
            };
        };


        class Disassembler {
        public:
            static InstDecoder* get(Arch arch);
            static InstDecoder* get();
        };
    }
}
