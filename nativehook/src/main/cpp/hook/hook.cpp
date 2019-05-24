//
// Created by swift on 2019/5/14.
//

#include "hook.h"

#if defined(__arm__)
#include "hook_arm32.h"
#elif defined(__aarch64__)
#include "hook_arm64.h"
#endif

using namespace SandHook::Hook;

AndroidCodeBuffer* InlineHook::backupBuffer = new AndroidCodeBuffer();

#if defined(__arm__)
InlineHook* InlineHook::instance = new InlineHookArm32Android();
#elif defined(__aarch64__)
InlineHook* InlineHook::instance = new InlineHookArm64Android();
#endif