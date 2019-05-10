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

        template <typename Raw>
        class Unit {
        public:

            Unit() {
                if (unitType() != Void) {
                    raw = reinterpret_cast<Raw *>(malloc(size()));
                    memset(raw, 0, size());
                    auto_alloc = true;
                }
            }

            Unit<Raw>(Raw *raw) : raw(raw) {}

            Unit<Raw>(Raw raw) {
                Unit();
                *this->raw = raw;
            }

            virtual void* getPC() {
                return auto_alloc ? nullptr : raw;
            }

            inline Raw* get() const {
                return raw;
            }

            inline void set(Raw raw) const {
                *this->raw = raw;
            }

            inline void copy(void* dest) {
                memcpy(dest, getPC(), size());
            }

            virtual UnitType unitType() {
                return UnitType::Unkown;
            };

            virtual U32 size() {
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

        template <typename DType>
        class Data : public Unit<DType> {
        public:
            Data(DType raw) : Unit<DType>(raw) {}
            inline UnitType unitType() override {
                return UnitType::Data;
            };
        };

        class Data16 : public Data<U16> {
        public:
            Data16(U16 raw) : Data(raw) {}
        };

        class Data32 : public Data<U32> {
        public:
            Data32(U32 raw) : Data(raw) {}
        };

        class Data64 : public Data<U64> {
        public:
            Data64(U64 raw) : Data(raw) {}
        };

        class Label : public Unit<None> {
        public:
            Label() {}
            inline UnitType unitType() override {
                return UnitType::Label;
            }
            U32 size() override {
                return 0;
            }
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
