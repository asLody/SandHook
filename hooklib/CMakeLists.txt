# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             sandhook

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/sandhook.cpp
             src/main/cpp/trampoline/trampoline.cpp
             src/main/cpp/trampoline/trampoline_manager.cpp
             src/main/cpp/utils/dlfcn_nougat.cpp
             src/main/cpp/utils/hide_api.cpp
             src/main/cpp/utils/utils.cpp
             src/main/cpp/utils/offset.cpp
             src/main/cpp/utils/elf_util.cpp
             src/main/cpp/casts/cast_art_method.cpp
             src/main/cpp/casts/cast_compiler_options.cpp
             src/main/cpp/art/art_method.cpp
             src/main/cpp/art/art_compiler_options.cpp
             src/main/cpp/trampoline/arch/arm32.S
             src/main/cpp/trampoline/arch/arm64.S
             src/main/cpp/inst/insts_arm32.cpp
             src/main/cpp/inst/insts_arm64.cpp
             src/main/cpp/nativehook/native_hook.cpp
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
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       sandhook

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )


add_definitions(-std=c++11)

ENABLE_LANGUAGE(ASM)