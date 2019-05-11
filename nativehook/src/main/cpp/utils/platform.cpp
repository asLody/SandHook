//
// Created by swift on 2019/5/11.
//

#include <unistd.h>
#include "platform.h"

bool flushCache(Addr addr, Off len) {
#if defined(__arm__)
    int i = cacheflush(addr, addr + len, 0);
                if (i == -1) {
                    return false;
                }
                return true;
#elif defined(__aarch64__)
    char *begin = reinterpret_cast<char *>(addr);
    __builtin___clear_cache(begin, begin + len);
#endif
    return true;
}