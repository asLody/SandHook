//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_INSTRUCTION_H
#define SANDHOOK_INSTRUCTION_H

#include "unit.h"

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


#define INST_CHECK(X,V) \
CHECK(X,V, valid = false;)

#define INST_DCHECK(X,V) \
DCHECK(X,V, valid = false;)

#define INST_ASSERT(COND) \
if (COND) { \
    valid = false; \
}

namespace SandHook {
    namespace Asm {

        template <typename Inst>
        class Instruction : public Unit<Inst> {
        public:

            Instruction() {}

            Instruction(Inst *inst) : Unit<Inst>(inst) {}

            UnitType unitType() override {
                return UnitType::Inst;
            };

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

            inline bool isValid() const {
                return valid;
            }

            virtual void decode(Inst* inst) {}

            virtual void assembler() {}

        protected:
            bool valid = true;
        };

        class Void : public Unit<None> {
        public:
            Void(U32 size) : size_(size) {}

            U32 size() override {
                return size_;
            }

        private:
            U32 size_;
        };

    }
}

#endif //SANDHOOK_INSTRUCTION_H
