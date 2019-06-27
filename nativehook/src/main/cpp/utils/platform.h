//
// Created by swift on 2019/5/11.
//

#pragma once

#include "base.h"

extern "C" bool flushCache(Addr addr, Off len);

extern "C" bool memUnprotect(Addr addr, Addr len);