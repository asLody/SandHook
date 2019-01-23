#include "base.h"

#if defined(__arm__)

#define Reg0 ip
//need restore
#define RegT ip
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
    add Reg0, Reg0, SIZE_JUMP
    mov pc, Reg0
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

//thumb-2
FUNCTION_START_T(DIRECT_JUMP_TRAMPOLINE_T)
    ldr pc, addr_target_t
addr_target_t:
    .long 0
FUNCTION_END(DIRECT_JUMP_TRAMPOLINE_T)


FUNCTION_START_T(INLINE_HOOK_TRAMPOLINE_T)
    ldr RegT, addr_origin_art_method_t
    cmp RegMethod, RegT
    bne origin_code_t
    ldr RegMethod, addr_hook_art_method_t
    ldr RegT, offset_entry_code2_t
    add RegT, RegMethod, RegT
    ldr pc, [RegT]
origin_code_t:
    .long 0
    .long 0
    ldr RegT, addr_origin_art_method_t
    add RegT, RegT, #8
    ldr RegT, [RegT]
    add RegT, RegT, SIZE_JUMP
    mov pc, RegT
addr_origin_art_method_t:
    .long 0
offset_entry_code2_t:
    .long 0
addr_hook_art_method_t:
    .long 0
FUNCTION_END(INLINE_HOOK_TRAMPOLINE_T)

FUNCTION_START_T(CALL_ORIGIN_TRAMPOLINE_T)
    ldr RegMethod, origin_art_method_t
    ldr pc, addr_origin_t
origin_art_method_t:
    .long 0
addr_origin_t:
    .long 0
FUNCTION_END(CALL_ORIGIN_TRAMPOLINE_T)

#endif