//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_INSTRUCTION_H
#define SANDHOOK_INSTRUCTION_H

#include <malloc.h>
#include "../includes/base.h"

//aarch64
typedef U32 InstA64;
//arm32
typedef U32 InstA32;
//thumb16
typedef U16 InstT16;
//thumb32
typedef U32 InstT32;

#if defined(__aarch64__)
typedef U32 InstRaw;
#endif

namespace SandHook {
    namespace Asm {

        template <typename Raw>
        class Unit {
        public:

            Unit() {
                raw = reinterpret_cast<Raw*>(malloc(sizeof(Raw)));
                auto_alloc = true;
            }

            Unit<Raw>(Raw *raw) : raw(raw) {}

            Unit<Raw>(Raw raw) {
                Unit();
                *this->raw = raw;
            }

            inline void* getPC() const {
                return raw;
            }

            Raw* get() const {
                return raw;
            }

            void set(Raw raw) const {
                *this->raw = raw;
            }

            inline void copy(void* dest) {
                memcpy(dest, getPC(), size());
            }

            virtual U8 size() {
                return sizeof(Raw);
            }

            virtual ~Unit() {
                if (auto_alloc) {
                    free(raw);
                }
            }

        private:
            Raw* raw;
            bool auto_alloc = false;
        };

        template <typename Inst>
        class Instruction : public Unit<Inst> {
        public:

            Instruction() {}

            Instruction(Inst *inst) : Unit<Inst>(inst) {}

            virtual InstType instType() {
                return unkownInst;
            }

            virtual Arch arch() {
                return unkownArch;
            }

            virtual U32 instCode() {
                return 0;
            };

            virtual bool pcRelate() {
                return false;
            }

            virtual void decode(Inst* decode) {}

            virtual void assembler() {}
        };

        class Data16 : public Unit<U16> {
        public:
            Data16(U16 raw) : Unit(raw) {}
        };

        class Data32 : public Unit<U32> {
        public:
            Data32(U32 raw) : Unit(raw) {}
        };

        class Data64 : public Unit<U64> {
        public:
            Data64(U64 raw) : Unit(raw) {}
        };

    }
}

#endif //SANDHOOK_INSTRUCTION_H
