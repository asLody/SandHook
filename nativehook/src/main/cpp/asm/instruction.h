//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_INSTRUCTION_H
#define SANDHOOK_INSTRUCTION_H

#include "unit.h"
#include "label.h"

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
        class Instruction : public Unit<Inst>, public LabelBinder {
        public:
            Instruction() {}

            Instruction(Inst *inst) : Unit<Inst>(inst) {}

            UnitType unitType() override {
                return UnitType::UnitInst;
            };

            virtual InstType instType() {
                return unknowInst;
            }

            virtual Arch arch() {
                return unknowArch;
            }

            virtual U32 instCode() {
                return 0;
            };

            virtual bool pcRelate() {
                return false;
            }

            virtual bool unknow() {
                return false;
            }

            inline bool isValid() const {
                return valid;
            }

            virtual void onOffsetApply(Off offset) {}

            void onLabelApply(Addr pc) override {
                onOffsetApply(pc - this->getVPC());
            }

            inline void bindLabel(Label &l) {
                label = &l;
                l.addBinder(this);
            }

            virtual void decode(Inst* inst) {
                inst_backup = *inst;
            }

            virtual void assembler() {
                this->set(inst_backup);
            }

        protected:
            bool valid = true;
            Label* label = nullptr;
            Inst inst_backup;
        };

        class Void : public Unit<Base> {
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
