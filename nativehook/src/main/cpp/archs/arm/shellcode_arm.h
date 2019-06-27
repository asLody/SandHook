//
// Created by swift on 2019/6/1.
//

#pragma once

//for shell code
#define FUNCTION_START(x) \
.align 4; \
.global x; \
.global x##_END; \
x:

#define FUNCTION_START_T(x) \
.syntax unified; \
.align 4; \
.thumb; \
.thumb_func; \
.global x; \
.global x##_END; \
x:

#define FUNCTION_END(x) x##_END: \
.size x, .-x \

#define IMPORT_SHELLCODE(x) extern "C" void x(); \
extern "C" void x##_END();

#define IMPORT_LABEL(X,T) extern T X;

#define SHELLCODE_LEN(x) (Addr)x##_END - (Addr)x