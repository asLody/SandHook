#include "base.h"

#if defined(__arm__)

FUNCTION_START(hook_trampoline)
    adr r0, t_i
    adr pc, addr_entry_code
addr_art_method:
    .long 0
addr_entry_code:
    .long 0
FUNCTION_END(hook_trampoline)

#endif
