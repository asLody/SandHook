//
// Created by swift on 2019/5/6.
//

#pragma once

// Helper to check whether the version of GCC used is greater than the specified
// requirement.
#define MAJOR 1000000
#define MINOR 1000
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#define GCC_VERSION_OR_NEWER(major, minor, patchlevel)                      \
  ((__GNUC__ * (MAJOR) + __GNUC_MINOR__ * (MINOR) + __GNUC_PATCHLEVEL__) >= \
   ((major) * (MAJOR) + ((minor)) * (MINOR) + (patchlevel)))
#elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#define GCC_VERSION_OR_NEWER(major, minor, patchlevel) \
  ((__GNUC__ * (MAJOR) + __GNUC_MINOR__ * (MINOR)) >=  \
   ((major) * (MAJOR) + ((minor)) * (MINOR) + (patchlevel)))
#else
#define GCC_VERSION_OR_NEWER(major, minor, patchlevel) 0
#endif


#if defined(__clang__) && !defined(VIXL_NO_COMPILER_BUILTINS)

// clang-format off
#define COMPILER_HAS_BUILTIN_CLRSB    (__has_builtin(__builtin_clrsb))
#define COMPILER_HAS_BUILTIN_CLZ      (__has_builtin(__builtin_clz))
#define COMPILER_HAS_BUILTIN_CTZ      (__has_builtin(__builtin_ctz))
#define COMPILER_HAS_BUILTIN_FFS      (__has_builtin(__builtin_ffs))
#define COMPILER_HAS_BUILTIN_POPCOUNT (__has_builtin(__builtin_popcount))
// clang-format on

#elif defined(__GNUC__) && !defined(VIXL_NO_COMPILER_BUILTINS)
// The documentation for these builtins is available at:
// https://gcc.gnu.org/onlinedocs/gcc-$MAJOR.$MINOR.$PATCHLEVEL/gcc//Other-Builtins.html

// clang-format off
# define COMPILER_HAS_BUILTIN_CLRSB    (GCC_VERSION_OR_NEWER(4, 7, 0))
# define COMPILER_HAS_BUILTIN_CLZ      (GCC_VERSION_OR_NEWER(3, 4, 0))
# define COMPILER_HAS_BUILTIN_CTZ      (GCC_VERSION_OR_NEWER(3, 4, 0))
# define COMPILER_HAS_BUILTIN_FFS      (GCC_VERSION_OR_NEWER(3, 4, 0))
# define COMPILER_HAS_BUILTIN_POPCOUNT (GCC_VERSION_OR_NEWER(3, 4, 0))
// clang-format on

#else
// One can define VIXL_NO_COMPILER_BUILTINS to force using the manually
// implemented C++ methods.

// clang-format off
#define COMPILER_HAS_BUILTIN_BSWAP    false
#define COMPILER_HAS_BUILTIN_CLRSB    false
#define COMPILER_HAS_BUILTIN_CLZ      false
#define COMPILER_HAS_BUILTIN_CTZ      false
#define COMPILER_HAS_BUILTIN_FFS      false
#define COMPILER_HAS_BUILTIN_POPCOUNT false
// clang-format on

#endif