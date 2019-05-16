//
// Created by swift on 2019/5/16.
//

#ifndef SANDHOOK_ARM32_BASE_H
#define SANDHOOK_ARM32_BASE_H

#include "arm_base.h"
#include "register_list_a32.h"

#define DECODE_OFFSET(bits, ext) signExtend32(bits + ext, COMBINE(get()->imm##bits, 0, ext))
#define ENCODE_OFFSET(bits, ext) get()->imm##bits = TruncateToUint##bits(offset >> ext)

using namespace SandHook::RegistersA32;

namespace SandHook {
    namespace AsmA32 {


        enum Sign { Plus, Minus };

        class MemOperand {
        public:
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
            RegisterA32* rn;    // base
            RegisterA32* rm;    // register offset
            S32 offset; // valid if rm_ == no_reg
            Shift shift;
            Sign sign;
            int shift_imm; // valid if rm_ != no_reg && rs_ == no_reg
            AddrMode addr_mode;   // bits P, U, and W
        };
    }
}

#endif //SANDHOOK_ARM32_BASE_H
