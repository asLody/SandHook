//
// Created by swift on 2019/5/11.
//

#include <unistd.h>
#include <sys/mman.h>
#include "platform.h"

bool FlushCache(Addr addr, Off len) {
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


extern "C" bool MemUnprotect(Addr addr, Addr len) {
    long pagesize = PAGE_SIZE;
    unsigned alignment = (unsigned)((unsigned long long)addr % pagesize);
    int i = mprotect((void *) (addr - alignment), (size_t) (alignment + len),
                     PROT_READ | PROT_WRITE | PROT_EXEC);
    return i != -1;
}