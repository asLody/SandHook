//
// Created by SwiftGan on 2019/4/15.
//

#pragma once

#include <memory>
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
CHECK(X,V, valid_ = false;)

#define INST_DCHECK(X,V) \
DCHECK(X,V, valid_ = false;)

#define INST_ASSERT(COND) \
if (COND) { \
    valid_ = false; \
}

namespace SandHook {
    namespace Asm {

        template <typename S, U32 C>
        class Instruction : public Unit<S>, public LabelBinder {
        public:

            Instruction() {}

            Instruction(void *inst) : Unit<S>(inst) {}

            Instruction(S *inst) : Unit<S>(inst) {}

            UnitTypeDef UnitType() override {
                return UnitTypeDef::UnitInst;
            };

            virtual InstType InstType() {
                return unknowInst;
            }

            virtual Arch Arch() {
                return unknowArch;
            }

            virtual U32 InstCode() {
                return C;
            };

            virtual bool PcRelate() {
                return false;
            }

            virtual bool Unknow() {
                return false;
            }

            inline bool Valid() const {
                return valid_;
            }

            virtual void OnOffsetApply(Off offset) {}

            void OnLabelApply(Addr pc) override {
                OnOffsetApply(pc - this->GetVPC());
            }

            INLINE void BindLabel(Label *label) {
                label_ = label;
                label->AddBind(this);
            }

            virtual void Disassemble() {
                backup_ = *this->pc_;
            }

            virtual void Assemble() {
                *this->pc_ = backup_;
            }

        protected:
            bool valid_ = true;
            Label* label_ = nullptr;
            S backup_;
        };

        class Void : public BaseUnit {
        public:
            Void(U32 size) : size_(size) {}

            U32 Size() override {
                return size_;
            }

        private:
            U32 size_;
        };

        using BaseInst = Instruction<Base,0>;
        using BaseInstRef = std::shared_ptr<BaseInst>;

    }
}
