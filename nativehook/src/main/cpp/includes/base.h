//
// Created by SwiftGan on 2019/4/15.
//

#ifndef SANDHOOK_BASE_H
#define SANDHOOK_BASE_H

#include <cstdint>
#include <cstring>
#include "compiler.h"

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef size_t ADDR;

const int PTR_BYTE = sizeof(void*);

#define PAGE_OFFSET 12

const int BITS_OF_BYTE = 8;

const ADDR PAGE_SIZE = 2 << PAGE_OFFSET;

enum Arch {
    arm32,
    arm64,
    unkownArch
};

enum InstType {
    A32,
    thumb16,
    thumb32,
    A64,
    unkownInst
};

template <int SizeInBits>
struct Unsigned;

template <>
struct Unsigned<16> {
    typedef U16 type;
};

template <>
struct Unsigned<32> {
    typedef U32 type;
};

template <>
struct Unsigned<64> {
    typedef U64 type;
};

template <typename T>
T AlignDown(T pointer,
            typename Unsigned<sizeof(T) * BITS_OF_BYTE>::type alignment) {
    // Use C-style casts to get static_cast behaviour for integral types (T), and
    // reinterpret_cast behaviour for other types.

    typename Unsigned<sizeof(T)* BITS_OF_BYTE>::type pointer_raw =
            (typename Unsigned<sizeof(T) * BITS_OF_BYTE>::type)pointer;

    size_t mask = alignment - 1;
    return (T)(pointer_raw & ~mask);
}

#define OFFSET(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)


//data covert

// Macros for compile-time format checking.
#if GCC_VERSION_OR_NEWER(4, 4, 0)
#define PRINTF_CHECK(format_index, varargs_index) \
  __attribute__((format(gnu_printf, format_index, varargs_index)))
#else
#define PRINTF_CHECK(format_index, varargs_index)
#endif

#ifdef __GNUC__
#define VIXL_HAS_DEPRECATED_WITH_MSG
#elif defined(__clang__)
#ifdef __has_extension(attribute_deprecated_with_message)
#define VIXL_HAS_DEPRECATED_WITH_MSG
#endif
#endif

#ifdef VIXL_HAS_DEPRECATED_WITH_MSG
#define VIXL_DEPRECATED(replaced_by, declarator) \
  __attribute__((deprecated("Use \"" replaced_by "\" instead"))) declarator
#else
#define VIXL_DEPRECATED(replaced_by, declarator) declarator
#endif


inline uint64_t TruncateToUintN(unsigned n, uint64_t x) {
    return static_cast<uint64_t>(x) & ((UINT64_C(1) << n) - 1);
}

VIXL_DEPRECATED("TruncateToUintN",
                inline uint64_t truncate_to_intn(unsigned n, int64_t x)) {
    return TruncateToUintN(n, x);
}


// clang-format off
#define INT_1_TO_32_LIST(V)                                                    \
V(1)  V(2)  V(3)  V(4)  V(5)  V(6)  V(7)  V(8)                                 \
V(9)  V(10) V(11) V(12) V(13) V(14) V(15) V(16)                                \
V(17) V(18) V(19) V(20) V(21) V(22) V(23) V(24)                                \
V(25) V(26) V(27) V(28) V(29) V(30) V(31) V(32)

#define INT_33_TO_63_LIST(V)                                                   \
V(33) V(34) V(35) V(36) V(37) V(38) V(39) V(40)                                \
V(41) V(42) V(43) V(44) V(45) V(46) V(47) V(48)                                \
V(49) V(50) V(51) V(52) V(53) V(54) V(55) V(56)                                \
V(57) V(58) V(59) V(60) V(61) V(62) V(63)

#define INT_1_TO_63_LIST(V) INT_1_TO_32_LIST(V) INT_33_TO_63_LIST(V)

#define DECLARE_TRUNCATE_TO_UINT_32(N)                             \
  inline uint32_t TruncateToUint##N(uint64_t x) {                  \
    return static_cast<uint32_t>(TruncateToUintN(N, x));           \
  }                                                                \
  VIXL_DEPRECATED("TruncateToUint" #N,                             \
                  inline uint32_t truncate_to_int##N(int64_t x)) { \
    return TruncateToUint##N(x);                                   \
  }

INT_1_TO_32_LIST(DECLARE_TRUNCATE_TO_UINT_32)
#undef DECLARE_TRUNCATE_TO_INT_N

// Bit field extraction.
inline uint64_t ExtractUnsignedBitfield64(int msb, int lsb, uint64_t x) {
    if ((msb == 63) && (lsb == 0)) return x;
    return (x >> lsb) & ((static_cast<uint64_t>(1) << (1 + msb - lsb)) - 1);
}


inline int64_t ExtractSignedBitfield64(int msb, int lsb, int64_t x) {
    uint64_t temp = ExtractUnsignedBitfield64(msb, lsb, x);
    // If the highest extracted bit is set, sign extend.
    if ((temp >> (msb - lsb)) == 1) {
        temp |= ~UINT64_C(0) << (msb - lsb);
    }
    int64_t result;
    memcpy(&result, &temp, sizeof(result));
    return result;
}

inline int32_t ExtractSignedBitfield32(int msb, int lsb, int32_t x) {
    uint32_t temp = TruncateToUint32(ExtractSignedBitfield64(msb, lsb, x));
    int32_t result;
    memcpy(&result, &temp, sizeof(result));
    return result;
}


static inline int16_t BITS16L(int32_t value) {
    return static_cast<int16_t>(value);
}

static inline int16_t BITS16H(int32_t value) {
    return static_cast<int16_t>(value >> 16);
}

static inline int32_t BITS32L(int64_t value) {
    return static_cast<int32_t>(value);
}

static inline int32_t BITS32H(int64_t value) {
    return static_cast<int32_t>(value >> 32);
}

// left/right shift
#define LFT(a, b, c) ((a & ((1 << b) - 1)) << c)
#define RHT(a, b, c) ((a >> c) & ((1 << b) - 1))

#endif //SANDHOOK_BASE_H
