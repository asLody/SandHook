#include "base.h"

#if defined(__arm__)

#define Reg0 ip
//need restore
#define Reg1 r11
#define RegMethod r0

FUNCTION_START(REPLACEMENT_HOOK_TRAMPOLINE)
    ldr RegMethod, addr_art_method
    ldr Reg0, offset_entry_code
    add Reg0, RegMethod, Reg0
    ldr pc, [Reg0]
addr_art_method:
    .long 0
offset_entry_code:
    .long 0
FUNCTION_END(REPLACEMENT_HOOK_TRAMPOLINE)


#define SIZE_JUMP #0x8
FUNCTION_START(DIRECT_JUMP_TRAMPOLINE)
    ldr pc, addr_target
addr_target:
    .long 0
FUNCTION_END(DIRECT_JUMP_TRAMPOLINE)



FUNCTION_START(INLINE_HOOK_TRAMPOLINE)
    ldr Reg0, addr_origin_art_method
    cmp RegMethod, Reg0
    bne origin_code
    ldr RegMethod, addr_hook_art_method
    ldr Reg0, offset_entry_code2
    add Reg0, RegMethod, Reg0
    ldr pc, [Reg0]
origin_code:
    .long 0
    .long 0
    ldr Reg0, addr_origin_art_method
    add Reg0, Reg0, #8
    ldr Reg0, [Reg0]
    add pc, Reg0, SIZE_JUMP
addr_origin_art_method:
    .long 0
offset_entry_code2:
    .long 0
addr_hook_art_method:
    .long 0
FUNCTION_END(INLINE_HOOK_TRAMPOLINE)



FUNCTION_START(CALL_ORIGIN_TRAMPOLINE)
    ldr RegMethod, origin_art_method
    ldr pc, addr_origin
origin_art_method:
    .long 0
addr_origin:
    .long 0
FUNCTION_END(CALL_ORIGIN_TRAMPOLINE)

#endif