//
// Created by swift on 2019/5/6.
//

#ifndef SANDHOOK_NH_DECODER_ARM64_H
#define SANDHOOK_NH_DECODER_ARM64_H

#include "decoder.h"

namespace SandHook {
    namespace Decoder {

        class Arm64Decoder : public InstDecoder {
        public:
            void decode(void *codeStart, Addr codeLen, InstVisitor &visitor) override;
        public:
            static Arm64Decoder* instant;
        };

    }
}

#endif //SANDHOOK_NH_DECODER_ARM64_H
