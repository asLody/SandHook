# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             sandhook-native

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/sandhook_native.cpp
             src/main/cpp/decoder/decoder.cpp
             src/main/cpp/archs/arm64/inst/inst_arm64.cpp
             src/main/cpp/archs/arm64/register/register_a64.cpp
             src/main/cpp/archs/arm64/register/register_list_a64.cpp
             src/main/cpp/archs/arm64/decoder/decoder_arm64.cpp
        )


include_directories(
        src/main/cpp/asm
        src/main/cpp/decoder
        src/main/cpp/elf
        src/main/cpp/includes
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
                       sandhook-native

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )


add_definitions(-std=c++11)

ENABLE_LANGUAGE(ASM)