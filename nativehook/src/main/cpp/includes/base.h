//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_BASE_H
#define SANDHOOK_BASE_H

#include <cstdint>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef size_t Address;

enum Arch {
    arm32,
    arm64,
    x86,
    x64,
    mips
};

enum InstType {
    arm32,
    thumb16,
    thumb32,
    aarch64,
    x86,
    x64
};

#endif //SANDHOOK_BASE_H
