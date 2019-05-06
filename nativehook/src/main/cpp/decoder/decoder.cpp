//
// Created by swift on 2019/5/6.
//

#include "decoder.h"

using namespace SandHook::Decoder;

InstDecoder* Decoder::get(Arch arch) {
    switch (arch) {
        case arm32:
            break;
        case arm64:
            break;
        default:
            return nullptr;
    }
    return nullptr;
}