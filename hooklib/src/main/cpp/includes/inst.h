//
// Created by swift on 2019/2/3.
//

#ifndef SANDHOOK_INST_VISTOR_H
#define SANDHOOK_INST_VISTOR_H

#include <cstdint>
#include "arch.h"

namespace SandHook {

    union Arm32Code {
        uint32_t code;
        struct {
            uint32_t cond:4;
            uint32_t empty:2;
            uint32_t opcode:4;
            uint32_t s:1;
            uint32_t rn:4;
            uint32_t rd:4;
            uint32_t operand2:12;
        } units;
    };

    enum InstArch {
        ARM32,
        Thumb,
        Thumb2,
        AARCH64,
        X86,
        X64
    };


    enum InstType {

    };

    class Inst {
    public:
        uint32_t opcode;
        InstArch arch;
        InstType type;

        virtual int instLen() const = 0;
    };

    class InstVisitor {
    public:
        virtual bool visit(Inst inst, Size offset, Size length) const = 0;
    };

}

#endif //SANDHOOK_INST_VISTOR_H
