#include "base.h"


#if defined(__aarch64__)

#define Reg0 x16
#define Reg1 x17
#define RegMethod x0

FUNCTION_START(REPLACEMENT_HOOK_TRAMPOLINE)
    ldr RegMethod, addr_art_method
    ldr Reg0, offset_entry_code
    add Reg0, RegMethod, Reg0
    ldr Reg0, [Reg0]
    br Reg0
addr_art_method:
    .long 0
    .long 0
offset_entry_code:
    .long 0
    .long 0
FUNCTION_END(REPLACEMENT_HOOK_TRAMPOLINE)

FUNCTION_START(DIRECT_JUMP_TRAMPOLINE)
    ldr Reg0, addr_target
    br Reg0
addr_target:
    .long 0
    .long 0
FUNCTION_END(DIRECT_JUMP_TRAMPOLINE)

FUNCTION_START(INLINE_HOOK_TRAMPOLINE)
    ldr Reg1, addr_origin_art_method
    cmp RegMethod, Reg1
    bne go_to_origin_code
    ldr Reg0, addr_hook_art_method
    ldr Reg1, offset_entry_code2
    add Reg1, Reg0, Reg1
    ldr Reg1, [Reg1]
    br Reg1
go_to_origin_code:
    mov RegMethod, Reg1
    b origin_code
addr_origin_art_method:
    .long 0
    .long 0
origin_code:
    .long 0
    .long 0
    .long 0
    .long 0
    ldr Reg1, offset_entry_code2
    add Reg1, RegMethod, Reg1
    ldr Reg1, [Reg1]
    br Reg1
offset_entry_code2:
    .long 0
    .long 0
addr_hook_art_method:
    .long 0
    .long 0
FUNCTION_END(INLINE_HOOK_TRAMPOLINE)

#endif
