//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_INSTRUCTION_H
#define SANDHOOK_INSTRUCTION_H

#include "../includes/base.h"

typedef U32 InstA64;
typedef U32 InstA32;
typedef U16 InstT16;
typedef U32 InstT32;

#if defined(__aarch64__)
typedef U32 InstRaw;
#endif

namespace SandHook {
    namespace Asm {

        template <typename Raw>
        class Instruction {
        public:

            inline void* getPC() const {
                return (void *) this;
            }

            bool pcRelate() {
                return false;
            }

            Raw get() {
                return *reinterpret_cast<Raw*>(this);
            }

            void set(Raw raw) {
                *this = raw;
            }

            U8 size() {
                return sizeof(Raw);
            }

            bool operator&(const Instruction &rhs) {
                return rhs == *this;
            }

            bool operator==(const Instruction &rhs) {
                return rhs == *this;
            }

            bool operator!=(const Instruction &rhs) {
                return !(rhs == *this);
            }

            virtual ~Instruction() {

            }
        };

    }
}

#endif //SANDHOOK_INSTRUCTION_H
