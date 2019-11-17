# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
if(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
    set(OS_DEPENDENDED_SRC
            archs/arm/arm64/assembler/assembler_arm64.cpp
            archs/arm/arm64/inst/inst_arm64.cpp
            archs/arm/arm64/register/register_arm64.cpp
            archs/arm/arm64/register/register_list_arm64.cpp
            archs/arm/arm64/decoder/decoder_arm64.cpp
            archs/arm/arm64/relocate/code_relocate_arm64.cpp
            archs/arm/arm64/hook/hook_arm64.cpp)

elseif (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "armeabi-v7a")
    set(OS_DEPENDENDED_SRC
            archs/arm/arm32/inst/inst_arm32.cpp
            archs/arm/arm32/inst/inst_t32.cpp
            archs/arm/arm32/inst/inst_t16.cpp
            archs/arm/arm32/register/register_arm32.cpp
            archs/arm/arm32/register/register_list_arm32.cpp
            archs/arm/arm32/assembler/assembler_arm32.cpp
            archs/arm/arm32/decoder/decoder_arm32.cpp
            archs/arm/arm32/hook/hook_arm32.cpp
            archs/arm/arm32/hook/breakpoint_shellcode.S
            archs/arm/arm32/relocate/code_relocate_arm32.cpp)
endif()

add_library( # Sets the name of the library.
        sandhook-native

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        sandhook_native.cpp
        decoder/decoder.cpp
        relocate/code_relocate.cpp
        elf/elf.cpp
        assembler/assembler.cpp
        buffer/code_buffer.cpp
        utils/platform.cpp
        hook/hook.cpp
        ${OS_DEPENDENDED_SRC})


include_directories(
        asm
        decoder
        elf
        utils
        includes
        buffer
        archs/arm
        archs/arm/arm64/inst
        archs/arm/arm64/register
        archs/arm/arm64/decoder
        archs/arm/arm64/assembler
        archs/arm/arm64/relocate
        archs/arm/arm64/hook
        archs/arm/arm32/inst
        archs/arm/arm32/register
        archs/arm/arm32/assembler
        archs/arm/arm32/decoder
        archs/arm/arm32/hook
        archs/arm/arm32/relocate
        src/main/cpp/antihook
)

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
        sandhook-native

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})


add_definitions(-std=c++11)

ENABLE_LANGUAGE(ASM)