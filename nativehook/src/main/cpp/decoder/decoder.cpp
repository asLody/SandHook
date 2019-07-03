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

bool DefaultVisitor::Visit(Unit<Base> *unit, void *pc) {
    bool res = visitor(unit, pc);
    delete unit;
    return res;
}

DefaultVisitor::DefaultVisitor(bool (*visitor)(Unit<Base> *, void *)) : visitor(visitor) {}

//do not support now
InstDecoder* Disassembler::Get(Arch arch) {
    switch (arch) {
        case arm32:
            return Get();
        case arm64:
            return Get();
        default:
            return nullptr;
    }
}

InstDecoder *Disassembler::Get() {
#if defined(__arm__)
    return Arm32Decoder::instant;
#elif defined(__aarch64__)
    return Arm64Decoder::instant;
#else
    return nullptr;
#endif
}
