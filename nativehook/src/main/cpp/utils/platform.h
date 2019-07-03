//
// Created by swift on 2019/5/11.
//

#pragma once

#include "base.h"

extern "C" bool FlushCache(Addr addr, Off len);

extern "C" bool MemUnprotect(Addr addr, Addr len);