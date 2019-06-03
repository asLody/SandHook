# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 14)

enable_language(ASM)
SET(CMAKE_ASM_FLAGS "${CFLAGS}  -x assembler-with-cpp")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
if(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
    set(OS_DEPENDENDED_SRC
            src/main/cpp/libffi/aarch64/ffi_arm64.c
            src/main/cpp/libffi/aarch64/sysv_arm64.S)

elseif (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "armeabi-v7a")
    set(OS_DEPENDENDED_SRC
            src/main/cpp/libffi/arm/ffi_armv7.c
            src/main/cpp/libffi/arm/sysv_armv7.S)
endif()

add_library( # Sets the name of the library.
        sandhook-xp

        # Sets the library as a shared library.
        SHARED

        src/main/cpp/art_jni_trampoline.cpp
        # Provides a relative path to your source file(s).
        src/main/cpp/libffi/closures.c
        src/main/cpp/libffi/closures.c
        src/main/cpp/libffi/debug.c
        src/main/cpp/libffi/dlmalloc.c
        src/main/cpp/libffi/java_raw_api.c
        src/main/cpp/libffi/prep_cif.c
        src/main/cpp/libffi/raw_api.c
        src/main/cpp/libffi/types.c
        src/main/cpp/libffi/ffi_cxx.cc
        ${OS_DEPENDENDED_SRC})


include_directories(
        src/main/cpp
        src/main/cpp/libffi
        src/main/cpp/libffi/platform_include
        ../nativehook/src/main/cpp
)

add_library(sandhook-native
        SHARED
        IMPORTED)

set_target_properties(sandhook-native
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libsandhook-native.so)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        sandhook-xp

        sandhook-native
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

ENABLE_LANGUAGE(ASM)