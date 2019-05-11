//
// Created by swift on 2019/5/11.
//

#ifndef SANDHOOK_NH_PLATFORM_H
#define SANDHOOK_NH_PLATFORM_H

#include "base.h"

extern "C" bool flushCache(Addr addr, Off len);

extern "C" bool memUnprotect(Addr addr, Addr len);

#endif //SANDHOOK_NH_PLATFORM_H
