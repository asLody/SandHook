//
// Created by swift on 2019/5/16.
//

#pragma once

#include <ostream>
#include "arm_base.h"
#include "register_list_arm32.h"
#include "instruction.h"

#define DECODE_OFFSET(bits, ext) SignExtend32(bits + ext, COMBINE(Get()->imm##bits, 0, ext))
#define ENCODE_OFFSET(bits, ext) Get()->imm##bits = TruncateToUint##bits(offset >> ext)

#define CODE_OFFSET(I) I->offset + (I->InstType() == A32 ? 2 * 4 : 2 * 2)

using namespace SandHook::RegistersA32;

namespace SandHook {
    namespace AsmA32 {


        enum Sign { Plus, Minus };

        class MemOperand {
        public:

            explicit MemOperand()
                    : rn(&UnknowRegiser), rm(&UnknowRegiser), offset(0), addr_mode(Offset) {}

            explicit MemOperand(RegisterA32* rn, S32 offset = 0, AddrMode am = Offset)
                    : rn(rn), rm(&UnknowRegiser), offset(offset), addr_mode(am) {}

            explicit MemOperand(RegisterA32* rn, RegisterA32* rm, AddrMode am = Offset)
                    : rn(rn), rm(rm), shift(LSL), shift_imm(0), addr_mode(am) {
            }

            explicit MemOperand(RegisterA32* rn, RegisterA32* rm, Shift shift, int shift_imm, AddrMode am = Offset)
                    : rn(rn), rm(rm), shift(shift), shift_imm(shift_imm & 31), addr_mode(am) {
            }

            // =====
            bool IsImmediateOffset() const { return (addr_mode == Offset); }
            bool IsRegisterOffset() const { return (addr_mode == Offset); }
            bool IsPreIndex() const { return addr_mode == PreIndex; }
            bool IsPostIndex() const { return addr_mode == PostIndex; }

        public:
            RegisterA32* rn;    // base_
            RegisterA32* rm;    // register offset_
            S32 offset; // valid if rm_ == no_reg
            Shift shift;
            Sign sign;
            int shift_imm; // valid if rm_ != no_reg && rs_ == no_reg
            AddrMode addr_mode;   // bits P, U, and W
        };


        class RegisterList {
        public:
            RegisterList() : list_(0) {}
            RegisterList(RegisterA32& reg)  // NOLINT(runtime/explicit)
                    : list_(RegisterToList(reg)) {}
            RegisterList(RegisterA32& reg1, RegisterA32& reg2)
                    : list_(RegisterToList(reg1) | RegisterToList(reg2)) {}
            RegisterList(RegisterA32& reg1, RegisterA32& reg2, RegisterA32& reg3)
                    : list_(RegisterToList(reg1) | RegisterToList(reg2) |
                            RegisterToList(reg3)) {}
            RegisterList(RegisterA32& reg1, RegisterA32& reg2, RegisterA32& reg3, RegisterA32& reg4)
                    : list_(RegisterToList(reg1) | RegisterToList(reg2) |
                            RegisterToList(reg3) | RegisterToList(reg4)) {}
            explicit RegisterList(U16 list) : list_(list) {}
            U16 GetList() const { return list_; }
            void SetList(U16 list) { list_ = list; }
            bool Includes(RegisterA32& reg) const {
                return (list_ & RegisterToList(reg)) != 0;
            }
            void Combine(RegisterList& other) { list_ |= other.GetList(); }
            void Combine(RegisterA32& reg) { list_ |= RegisterToList(reg); }
            void Remove(RegisterList& other) { list_ &= ~other.GetList(); }
            void Remove(RegisterA32& reg) { list_ &= ~RegisterToList(reg); }
            bool Overlaps(RegisterList& other) const {
                return (list_ & other.list_) != 0;
            }
            bool IsR0toR7orPC() const {
                // True if all the registers from the list are not from r8-r14.
                return (list_ & 0x7f00) == 0;
            }
            bool IsR0toR7orLR() const {
                // True if all the registers from the list are not from r8-r13 nor from r15.
                return (list_ & 0xbf00) == 0;
            }
            bool IsEmpty() const { return list_ == 0; }
            static RegisterList Union(const RegisterList& list_1,
                                      const RegisterList& list_2) {
                return RegisterList(list_1.list_ | list_2.list_);
            }
            static RegisterList Union(const RegisterList& list_1,
                                      const RegisterList& list_2,
                                      const RegisterList& list_3) {
                return Union(list_1, Union(list_2, list_3));
            }
            static RegisterList Union(const RegisterList& list_1,
                                      const RegisterList& list_2,
                                      const RegisterList& list_3,
                                      const RegisterList& list_4) {
                return Union(Union(list_1, list_2), Union(list_3, list_4));
            }
            static RegisterList Intersection(const RegisterList& list_1,
                                             const RegisterList& list_2) {
                return RegisterList(list_1.list_ & list_2.list_);
            }
            static RegisterList Intersection(const RegisterList& list_1,
                                             const RegisterList& list_2,
                                             const RegisterList& list_3) {
                return Intersection(list_1, Intersection(list_2, list_3));
            }
            static RegisterList Intersection(const RegisterList& list_1,
                                             const RegisterList& list_2,
                                             const RegisterList& list_3,
                                             const RegisterList& list_4) {
                return Intersection(Intersection(list_1, list_2),
                                    Intersection(list_3, list_4));
            }

        private:
            static U16 RegisterToList(RegisterA32& reg) {
                if (reg.Code() == UnknowRegiser.Code()) {
                    return 0;
                } else {
                    return static_cast<U16>(UINT16_C(1) << reg.Code());
                }
            }

            // Bitfield representation of all registers in the list
            // (1 for r0, 2 for r1, 4 for r2, ...).
            U16 list_;
        };

        inline uint32_t GetRegisterListEncoding(const RegisterList& registers,
                                                int first,
                                                int count) {
            return (registers.GetList() >> first) & ((1 << count) - 1);
        }

        inline bool IsThumbCode(Addr codeAddr) {
            return (codeAddr & 0x1) == 0x1;
        }

        inline bool IsThumb32(InstT16 code) {
            return ((code & 0xF000) == 0xF000) || ((code & 0xF800) == 0xE800);
        }

        inline void* GetThumbCodeAddress(void *code) {
            Addr addr = reinterpret_cast<Addr>(code) & (~0x1);
            return reinterpret_cast<void*>(addr);
        }

        inline void* GetThumbPC(void *code) {
            Addr addr = reinterpret_cast<Addr>(code) & (~0x1);
            return reinterpret_cast<void*>(addr + 1);
        }

        std::ostream& operator<<(std::ostream& os, RegisterList registers);

    }
}