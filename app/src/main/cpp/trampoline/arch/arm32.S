#include "base.h"

#if defined(__arm__)

FUNCTION_START(REPLACEMENT_HOOK_TRAMPOLINE)
    ldr r0, addr_art_method
    ldr r12, offset_entry_code
    add r12, r0, r12
    ldr pc, [r12]
addr_art_method:
    .long 0
offset_entry_code:
    .long 0
FUNCTION_END(REPLACEMENT_HOOK_TRAMPOLINE)

FUNCTION_START(DIRECT_JUMP_TRAMPOLINE)
    ldr pc, addr_target
addr_target:
    .long 0
FUNCTION_END(DIRECT_JUMP_TRAMPOLINE)

FUNCTION_START(INLINE_HOOK_TRAMPOLINE)
FUNCTION_END(INLINE_HOOK_TRAMPOLINE)

FUNCTION_START(CALL_ORIGIN_TRAMPOLINE)
FUNCTION_END(CALL_ORIGIN_TRAMPOLINE)

#endif