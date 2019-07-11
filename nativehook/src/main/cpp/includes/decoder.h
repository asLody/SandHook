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
            virtual bool Visit(BaseUnit *unit, void *pc) {
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
            virtual void Disassemble(void *code_start, Addr code_len, InstVisitor &visitor,
                                     bool only_pc_rel = false) = 0;
            inline void Disassemble(void *codeStart, Addr codeLen,
                                    bool (*visitor)(BaseUnit *, void *), bool only_pc_rel = false) {
                InstVisitor vis = DefaultVisitor(visitor);
                Disassemble(codeStart, codeLen, vis, only_pc_rel);
            };
        };


        class Disassembler {
        public:
            static InstDecoder* Get(Arch arch);
            static InstDecoder* Get();
        };
    }
}
