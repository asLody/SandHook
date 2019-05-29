//
// Created by swift on 2019/5/16.
//

#ifndef SANDHOOK_ARM_BASE_H
#define SANDHOOK_ARM_BASE_H

// Condition codes.
enum Condition {
    eq = 0,   // Z set            Equal.
    ne = 1,   // Z clear          Not equal.
    cs = 2,   // C set            Carry set.
    cc = 3,   // C clear          Carry clear.
    mi = 4,   // N set            Negative.
    pl = 5,   // N clear          Positive or zero.
    vs = 6,   // V set            Overflow.
    vc = 7,   // V clear          No overflow.
    hi = 8,   // C set, Z clear   Unsigned higher.
    ls = 9,   // C clear or Z set Unsigned lower or same.
    ge = 10,  // N == V           Greater or equal.
    lt = 11,  // N != V           Less than.
    gt = 12,  // Z clear, N == V  Greater than.
    le = 13,  // Z set or N != V  Less then or equal
    al = 14,  //                  Always.
    nv = 15,  // Behaves as always/al.

    // Aliases.
    hs = cs,  // C set            Unsigned higher or same.
    lo = cc   // C clear          Unsigned lower.
};

enum Shift {
    NO_SHIFT = -1,
    LSL = 0x0,
    LSR = 0x1,
    ASR = 0x2,
    ROR = 0x3,
    MSL = 0x4,
    RRX = 0x4
};

enum AddrMode { Offset, PreIndex, PostIndex, NonAddrMode};


//decode field & encode field

//condition
#define DECODE_COND condition = Condition(inst->cond)
#define ENCODE_COND get()->cond = condition

//reg
#define DECODE_RD(Type) rd = Type(static_cast<U8>(get()->rd))
#define ENCODE_RD get()->rd = rd->getCode()

#define DECODE_RT(Type) rt = Type(static_cast<U8>(get()->rt))
#define ENCODE_RT get()->rt = rt->getCode()

#define DECODE_RM(Type) rm = Type(static_cast<U8>(get()->rm))
#define ENCODE_RM get()->rm = rm->getCode()

#define DECODE_RN(Type) rn = Type(static_cast<U8>(get()->rn))
#define ENCODE_RN get()->rn = rn->getCode()


//op
#define DECODE_OP op = OP(inst->op)
#define ENCODE_OP get()->op = op

#define DECODE_SHIFT operand.shift = Shift(inst->shift)
#define ENCODE_SHIFT get()->shift = operand.shift


#endif //SANDHOOK_ARM_BASE_H
