//
// Created by swift on 2019/5/6.
//

#pragma once

#include "decoder.h"

namespace SandHook {
    namespace Decoder {

        class Arm64Decoder : public InstDecoder {
        public:
            void Disassemble(void *codeStart, Addr codeLen, InstVisitor &visitor,
                             bool onlyPcRelInst) override;
        public:
            static Arm64Decoder* instant;
        };

    }
}