#include "base.h"


#if defined(__aarch64__)

FUNCTION_START(REPLACEMENT_HOOK_TRAMPOLINE)
    ldr x0, addr_art_method
    ldr x16, offset_entry_code
    add x16, x0, x16
    ldr x16, [x16]
    br x16
addr_art_method:
    .long 0
    .long 0
offset_entry_code:
    .long 0
    .long 0
FUNCTION_END(REPLACEMENT_HOOK_TRAMPOLINE)

FUNCTION_START(DIRECT_JUMP_TRAMPOLINE)
    ldr x16, addr_target
    br x16
addr_target:
    .long 0
    .long 0
FUNCTION_END(DIRECT_JUMP_TRAMPOLINE)

FUNCTION_START(INLINE_HOOK_TRAMPOLINE)
    ldr x17, addr_origin_art_method
    cmp x0, x17
    bne go_to_origin_code
    ldr x16, addr_hook_art_method
    ldr x17, offset_entry_code2
    add x17, x16, x17
    ldr x17, [x17]
    br x17
go_to_origin_code:
    mov x0, x17
    b origin_code
addr_origin_art_method:
    .long 0
    .long 0
origin_code:
    .long 0
    .long 0
    .long 0
    .long 0
    ldr x17, offset_entry_code2
    add x17, x0, x17
    ldr x17, [x17]
    br x17
offset_entry_code2:
    .long 0
    .long 0
addr_hook_art_method:
    .long 0
    .long 0
FUNCTION_END(INLINE_HOOK_TRAMPOLINE)

#endif
