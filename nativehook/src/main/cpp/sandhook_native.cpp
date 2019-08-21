//
// Created by SwiftGan on 2019/4/15.
//

#include <jni.h>
#include <log.h>
#include <unistd.h>
#include "sandhook_native.h"
#include "hook.h"
#include "elf.h"

using namespace SandHook::Hook;
using namespace SandHook::Elf;

int m1 = 5;
int m3 = 1036;
int m2 = 1035;
int m4 = 5;

void (*dosth3Backup)(int, int) = nullptr;
void (*dosth4Backup)() = nullptr;
void (*dosth4Backup2)() = nullptr;

void (*innerSuspendVM)() = nullptr;


static void (*heapPreForkBackup)(void *) = nullptr;


static void myHeapPreFork(void *heap) {
    heapPreForkBackup(heap);
}


void SuspendVMReplace() {
    LOGE("VM Suspend!");
    innerSuspendVM();
}

void do2() {
    int a = 1 + 1;
    int b = 1 + 1;
    int c = 1 + 1;
    int d = 1 + 1;
}

void do3(int x, int y) {
    do2();
    int a = 1 + 1;
    int b = 1 + 1;
    int c = 1 + 1;
    int d = a + b + c;
    LOGE("do3 = %d", y);
}

void do5() {
    LOGE("x = %d", 5);
}

void do4() {
    int a = 1 + 1;
    int b = a + 1;
    int d = a + 1;
    int e = a + 1;
}

static std::atomic_int count;

void do4replace() {


    LOGE("do4replace :%d", count++);

    int a = 1 + 1;
    int b = 1 + 1;
    int c = 1 + 1;
    int d = 1 + 1;
    dosth4Backup();
}

void do4replace2() {
    int a = 1 + 1;
    int c = 1 + 1;
    int d = 1 + 1;
    dosth4Backup2();
}

void do3replace(int x, int y) {
    int a = 1 + 1;
    int b = 1 + 1;
    int c = 1 + 1;
    int d = 1 + 1;
    dosth3Backup(x, y);
}

void do1() {
    do2();
}

void breakCallback(REG regs[]) {
    LOGE("breakCallback = %d SP = %d", regs[0], regs);
}


class Visitor : public InstVisitor {
    bool Visit(Unit<Base> *unit, void *pc) override {
        BaseInst* instruction = reinterpret_cast<BaseInst *>(unit);
        instruction->Assemble();
        return true;
    }
};

static void *ThreadEntry2(void *arg) {
    while(true) {
        *(int*)(0) = 1;
        LOGE("ThreadEntry2");
    }
    LOGE("ThreadEntry2");
    return nullptr;
}

static void *ThreadEntry(void *arg) {
    pthread_t pthread_ptr_;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024 * 128);
    pthread_create(&pthread_ptr_, &attr, ThreadEntry2, nullptr);
    while(true) {
        *(int*)(0) = 1;
        LOGE("ThreadEntry1");
    }
    return nullptr;
}

std::atomic_bool inited;

static void callback() {
    if (inited)
        return;
    inited = true;
    pthread_t pthread_ptr_;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024 * 128);
    pthread_create(&pthread_ptr_, &attr, ThreadEntry, nullptr);
}

typedef enum {
    SP_DEFAULT = -1,
    SP_BACKGROUND = 0,
    SP_FOREGROUND = 1,
    SP_SYSTEM = 2,  // can't be used with set_sched_policy()
    SP_AUDIO_APP = 3,
    SP_AUDIO_SYS = 4,
    SP_TOP_APP = 5,
    SP_RT_APP = 6,
    SP_RESTRICTED = 7,
    SP_CNT,
    SP_MAX = SP_CNT - 1,
    SP_SYSTEM_DEFAULT = SP_FOREGROUND,
} SchedPolicy;

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_nativehook_NativeHook_test(JNIEnv *env, jclass jclass1) {


//    dosth4Backup = reinterpret_cast<void (*)()>(InlineHook::instance->SingleInstHook(
//            reinterpret_cast<void *>(do4),
//            reinterpret_cast<void *>(do4replace)));
//
//    InlineHook::instance->callback = callback;
//
//
//    do4();
//
//    if (sizeof(void*) == 8) {
//        heapPreForkBackup = reinterpret_cast<void (*)(void *)>(SandSingleInstHookSym("/system/lib64/libart.so", "_ZN3art2gc4Heap13PreZygoteForkEv",
//                                                                                 reinterpret_cast<void *>(myHeapPreFork)));
//        innerSuspendVM = reinterpret_cast<void (*)()>(SandSingleInstHookSym("/system/lib64/libart.so", "_ZN3art3Dbg9SuspendVMEv",
//                                                                        reinterpret_cast<void *>(SuspendVMReplace)));
//    } else {
//        heapPreForkBackup = reinterpret_cast<void (*)(void *)>(SandSingleInstHookSym("/system/lib/libart.so", "_ZN3art2gc4Heap13PreZygoteForkEv",
//                                                                                 reinterpret_cast<void *>(myHeapPreFork)));
//        innerSuspendVM = reinterpret_cast<void (*)()>(SandSingleInstHookSym("/system/lib/libart.so", "_ZN3art3Dbg9SuspendVMEv",
//                                                                        reinterpret_cast<void *>(SuspendVMReplace)));
//    }
//
//    void* do3P = reinterpret_cast<void *>(do3);
//
//    InlineHook::instance->BreakPoint(reinterpret_cast<void *>((Addr)do3), breakCallback);
//
//    LOGE("ok");
//
//    do3(100, 2);


}

extern "C"
EXPORT void* SandGetSym(const char* so, const char* symb) {
    ElfImg elfImg(so);
    return reinterpret_cast<void *>(elfImg.GetSymAddress(symb));
}

extern "C"
EXPORT void* SandInlineHook(void* origin, void* replace) {
    return InlineHook::instance->Hook(origin, replace);
}

extern "C"
EXPORT void* SandInlineHookSym(const char* so, const char* symb, void* replace) {
    ElfImg elfImg(so);
    void* origin = reinterpret_cast<void *>(elfImg.GetSymAddress(symb));

    if (origin == nullptr)
        return nullptr;
    return InlineHook::instance->Hook(origin, replace);
}

extern "C"
EXPORT void* SandSingleInstHook(void* origin, void* replace) {
    return InlineHook::instance->SingleInstHook(origin, replace);
}

extern "C"
EXPORT void* SandSingleInstHookSym(const char* so, const char* symb, void* replace) {
    ElfImg elfImg(so);
    void* origin = reinterpret_cast<void *>(elfImg.GetSymAddress(symb));

    if (origin == nullptr)
        return nullptr;
    return InlineHook::instance->SingleInstHook(origin, replace);
}

extern "C"
EXPORT bool SandBreakpoint(void* origin, void (*callback)(REG[])) {
    return InlineHook::instance->BreakPoint(origin, callback);
}