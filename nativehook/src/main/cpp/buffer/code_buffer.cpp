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

void *AndroidCodeBuffer::GetBuffer(U32 size) {
    AutoLock autoLock(alloc_space_lock);
    void* mmapRes;
    Addr exeSpace = 0;
    if (execute_space_list.size() == 0) {
        goto label_alloc_new_space;
    } else if (execute_page_offset + size + 4> current_execute_page_size) {
        goto label_alloc_new_space;
    } else {
        exeSpace = reinterpret_cast<Addr>(execute_space_list.back());
        //4 字节对齐
        Addr retSpace = RoundUp(exeSpace + execute_page_offset, 4);
        execute_page_offset = retSpace + size - exeSpace;
        return reinterpret_cast<void *>(retSpace);
    }
label_alloc_new_space:
    current_execute_page_size = static_cast<U32>(FIT(size, PAGE_SIZE));
    mmapRes = mmap(NULL, current_execute_page_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_ANON | MAP_PRIVATE, -1, 0);
    if (mmapRes == MAP_FAILED) {
        return 0;
    }
    memset(mmapRes, 0, current_execute_page_size);
    execute_space_list.push_back(mmapRes);
    execute_page_offset = size;
    return mmapRes;
}

AndroidCodeBuffer::AndroidCodeBuffer() {}

StaticCodeBuffer::StaticCodeBuffer(Addr pc) : pc(pc) {}

void *StaticCodeBuffer::GetBuffer(U32 bufferSize) {
    if (!MemUnprotect(pc, bufferSize)) {
        LOGE("error MemUnprotect!");
    }
    return reinterpret_cast<void *>(pc);
}

void AndroidReSizableBufferUnsafe::ResetLastBufferSize(U32 size) {
    if (execute_page_offset + (size - last_alloc_size) <= current_execute_page_size) {
        execute_page_offset += size - last_alloc_size;
        last_alloc_size = size;
    }
}

void *AndroidReSizableBufferUnsafe::GetBuffer(U32 bufferSize) {
    void* res = AndroidCodeBuffer::GetBuffer(bufferSize);
    if (res) {
        last_alloc_size = bufferSize;
    }
    return res;
}
