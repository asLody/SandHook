//
// Created by swift on 2019/5/16.
//

#pragma once

// Condition codes.
enum Condition {
    eq = 0,   // Z Set            Equal.
    ne = 1,   // Z clear          Not equal.
    cs = 2,   // C Set            Carry Set.
    cc = 3,   // C clear          Carry clear.
    mi = 4,   // N Set            Negative.
    pl = 5,   // N clear          Positive or zero.
    vs = 6,   // V Set            Overflow.
    vc = 7,   // V clear          No overflow.
    hi = 8,   // C Set, Z clear   Unsigned higher.
    ls = 9,   // C clear or Z Set Unsigned lower or same.
    ge = 10,  // N == V           Greater or equal.
    lt = 11,  // N != V           Less than.
    gt = 12,  // Z clear, N == V  Greater than.
    le = 13,  // Z Set or N != V  Less then or equal
    al = 14,  //                  Always.
    nv = 15,  // Behaves as always/al.

    // Aliases.
    hs = cs,  // C Set            Unsigned higher or same.
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


//Disassembler field & encode field

//condition
#define DECODE_COND condition = Condition(Get()->cond)
#define ENCODE_COND Get()->cond = condition

//reg_
#define DECODE_RD(Type) rd = Type(static_cast<U8>(Get()->rd))
#define ENCODE_RD Get()->rd = rd->Code()

#define DECODE_RT(Type) rt = Type(static_cast<U8>(Get()->rt))
#define ENCODE_RT Get()->rt = rt->Code()

#define DECODE_RM(Type) rm = Type(static_cast<U8>(Get()->rm))
#define ENCODE_RM Get()->rm = rm->Code()

#define DECODE_RN(Type) rn = Type(static_cast<U8>(Get()->rn))
#define ENCODE_RN Get()->rn = rn->Code()


//op
#define DECODE_OP op = OP(Get()->op)
#define ENCODE_OP Get()->op = op

#define DECODE_SHIFT operand.shift_ = Shift(Get()->shift)
#define ENCODE_SHIFT Get()->shift = operand.shift_