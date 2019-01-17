//
// Created by SwiftGan on 2019/1/17.
//

#ifndef SANDHOOK_BASE_H
#define SANDHOOK_BASE_H

#if defined(__WIN32__) || defined(__APPLE__)
#define cdecl(s) s
#else
#define cdecl(s) s
#endif

#define FUNCTION_START(x) .globl cdecl(x) \
cdecl(x):

#define FUNCTION_END(x) .x:

#endif //SANDHOOK_BASE_H
