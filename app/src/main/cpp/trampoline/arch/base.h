//
// Created by SwiftGan on 2019/1/17.
//

#ifndef SANDHOOK_BASE_H
#define SANDHOOK_BASE_H

#define FUNCTION_START(x) \
.text; \
.align 4; \
.global x; \
x: \

#define FUNCTION_END(x) .size x, .-x

#define REPLACEMENT_HOOK_TRAMPOLINE replacement_hook_trampoline
#define INLINE_HOOK_TRAMPOLINE inline_hook_trampoline
#define DIRECT_JUMP_TRAMPOLINE direct_jump_trampoline

#endif //SANDHOOK_BASE_H
