#include "base.h"

FUNCTION_START(hook_trampoline)
    adr r0, addr_art_method
    adr pc, addr_entry_code
addr_art_method:
    .long 0
addr_entry_code:
    .long 0
FUNCTION_END(hook_trampoline)
