//
// Created by swift on 2019/5/11.
//

#include <sys/mman.h>
#include <platform.h>
#include "log.h"
#include "code_buffer.h"
#include "lock.h"

using namespace SandHook::Assembler;
using namespace SandHook::Utils;

void *AndroidCodeBuffer::getBuffer(U32 size) {
    AutoLock autoLock(allocSpaceLock);
    void* mmapRes;
    Addr exeSpace = 0;
    if (executeSpaceList.size() == 0) {
        goto label_alloc_new_space;
    } else if (executePageOffset + size > currentExecutePageSize) {
        goto label_alloc_new_space;
    } else {
        exeSpace = reinterpret_cast<Addr>(executeSpaceList.back());
        Addr retSpace = exeSpace + executePageOffset;
        executePageOffset += size;
        return reinterpret_cast<void *>(retSpace);
    }
label_alloc_new_space:
    currentExecutePageSize = static_cast<U32>(FIT(size, P_SIZE));
    mmapRes = mmap(NULL, currentExecutePageSize, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_ANON | MAP_PRIVATE, -1, 0);
    if (mmapRes == MAP_FAILED) {
        return 0;
    }
    memset(mmapRes, 0, currentExecutePageSize);
    executeSpaceList.push_back(mmapRes);
    executePageOffset = size;
    return mmapRes;
}

AndroidCodeBuffer::AndroidCodeBuffer() {}

StaticCodeBuffer::StaticCodeBuffer(Addr pc) : pc(pc) {}

void *StaticCodeBuffer::getBuffer(U32 bufferSize) {
    if (!memUnprotect(pc, bufferSize)) {
        LOGE("error memUnprotect!");
    }
    return reinterpret_cast<void *>(pc);
}

void AndroidRellocBufferUnsafe::resetLastBufferSize(U32 size) {
    if (executePageOffset + (size - lastAllocSize) <= currentExecutePageSize) {
        executePageOffset += size - lastAllocSize;
        lastAllocSize = size;
    }
}

void *AndroidRellocBufferUnsafe::getBuffer(U32 bufferSize) {
    void* res = AndroidCodeBuffer::getBuffer(bufferSize);
    if (res) {
        lastAllocSize = bufferSize;
    }
    return res;
}
