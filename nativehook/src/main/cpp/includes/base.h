//
// Created by SwiftGan on 2019/4/15.
//

#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>
#include "compiler.h"

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

typedef size_t Addr;

using InstCode = U32;

//32bit
#if defined(__i386__) || defined(__arm__)
typedef S32 Off;
//64bit
#elif defined(__aarch64__) || defined(__x86_64__)
typedef S64 Off;
#endif

#define PAGE_OFFSET 12

const int BITS_OF_BYTE = 8;

#define P_SIZE PAGE_SIZE

enum Arch {
    arm32,
    arm64,
    unknowArch
};

enum UnitTypeDef {
    UnitInst,
    UnitData,
    UnitLabel,
    UnitVoid,
    UnitUnknow
};

enum InstType {
    A32,
    thumb16,
    thumb32,
    A64,
    unknowInst
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


struct Base {
};


#define INLINE __always_inline


template <typename T>
T AlignDown(T pointer,
            typename Unsigned<sizeof(T) * BITS_OF_BYTE>::type alignment) {
    // Use C-style casts to Get static_cast behaviour for integral types (T), and
    // reinterpret_cast behaviour for other types.

    typename Unsigned<sizeof(T)* BITS_OF_BYTE>::type pointer_raw =
            (typename Unsigned<sizeof(T) * BITS_OF_BYTE>::type)pointer;

    size_t mask = alignment - 1;
    return (T)(pointer_raw & ~mask);
}

#define ALIGN(value, align) ((uintptr_t)value & ~((uintptr_t)align - 1))

template<typename T>
struct Identity {
    using type = T;
};

template<typename T>
constexpr T RoundDown(T x, typename Identity<T>::type n) {
    return (x & -n);
}

template<typename T>
constexpr T RoundUp(T x, typename std::remove_reference<T>::type n) {
    return RoundDown(x + n - 1, n);
}

#define FIT(value, align) value <= align ? align : ((value / align) + align)

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


//只保留低N位
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

//位提取
// Bit field extraction.
inline U64 ExtractUnsignedBitfield64(int msb, int lsb, U64 x) {
    if ((msb == 63) && (lsb == 0)) return x;
    return (x >> lsb) & ((static_cast<U64>(1) << (1 + msb - lsb)) - 1);
}


inline S64 ExtractSignedBitfield64(int msb, int lsb, U64 x) {
    U64 temp = ExtractUnsignedBitfield64(msb, lsb, x);
    // If the highest extracted bit is Set, sign extend_.
    if ((temp >> (msb - lsb)) == 1) {
        temp |= ~UINT64_C(0) << (msb - lsb);
    }
    S64 result;
    memcpy(&result, &temp, sizeof(result));
    return result;
}

inline int32_t ExtractSignedBitfield32(int msb, int lsb, U32 x) {
    U32 temp = TruncateToUint32(ExtractSignedBitfield64(msb, lsb, x));
    S32 result;
    memcpy(&result, &temp, sizeof(result));
    return result;
}


template <typename T>
inline T SignExtend(T val, int bitSize) {
    T mask = (T(2) << (bitSize - 1)) - T(1);
    val &= mask;
    T sign_bits = -((val >> (bitSize - 1)) << bitSize);
    val |= sign_bits;
    return val;
}


#define BITS16L(value) static_cast<U16>(value & 0xffff)

#define BITS16H(value)  static_cast<U16>(value >> 16)

#define BITS32L(value) static_cast<U32>(value)

#define BITS32H(value) static_cast<U32>(value >> 32)

#define COMBINE(hi, lo, lowide) (hi << lowide) | lo

/* borrow from gdb, refer: binutils-gdb/gdb/Arch/arm.h */
#define SUB_MASK(x) ((1L << ((x) + 1)) - 1)
#define BITS(obj, st, fn) (((obj) >> (st)) & SUB_MASK((fn) - (st)))
#define BIT(obj, st) (((obj) >> (st)) & 1)
#define SBITS(obj, st, fn) ((long)(BITS(obj, st, fn) | ((long)BIT(obj, fn) * ~SUB_MASK(fn - st))))



#define DCHECK(X,V, ACTION) \
if (X == V) {   \
    ACTION  \
}

#define CHECK(X,V, ACTION) \
if (X != V) { \
   ACTION         \
}

#define ENUM_VALUE(Type, Value) static_cast<std::underlying_type<Type>::type>(Value)

static inline S64 SignExtend64(unsigned int bits, U64 value) {
    return ExtractSignedBitfield64(bits - 1, 0, value);
}

static inline S32 SignExtend32(unsigned int bits, U32 value) {
    return ExtractSignedBitfield32(bits - 1, 0, value);
}

template<typename U, typename T>
U ForceCast(T *x) {
    return (U) (uintptr_t) x;
}

template<typename U, typename T>
U ForceCast(T &x) {
    return *(U *) &x;
}

