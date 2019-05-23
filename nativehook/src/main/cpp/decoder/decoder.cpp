//
// Created by swift on 2019/5/6.
//

#include "decoder.h"
#if defined(__arm__)
#include "decoder_arm32.h"
#elif defined(__aarch64__)
#include "decoder_arm64.h"
#endif

using namespace SandHook::Decoder;

//do not support now
InstDecoder* Disassembler::get(Arch arch) {
    switch (arch) {
        case arm32:
            return get();
        case arm64:
            return get();
        default:
            return nullptr;
    }
}

InstDecoder *Disassembler::get() {
#if defined(__arm__)
    return Arm32Decoder::instant;
#elif defined(__aarch64__)
    return Arm64Decoder::instant;
#else
    return nullptr;
#endif
}
