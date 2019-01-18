#include "base.h"


#if defined(__aarch64__)

FUNCTION_START(hook_trampoline)
    adr x0, addr_art_method
    adr x16, addr_entry_code
    br x16
addr_art_method:
    .long 0
    .long 0
addr_entry_code:
    .long 0
    .long 0
FUNCTION_END(hook_trampoline)

#endif
