#include "base.h"

FUNCTION_START(hook_trampoline)
    ldr x0, addr_art_method
    ldr x16, addr_entry_code
    br x16
addr_art_method:
    .long 0
    .long 0
addr_entry_code:
    .long 0
    .long 0
FUNCTION_END(hook_trampoline)
