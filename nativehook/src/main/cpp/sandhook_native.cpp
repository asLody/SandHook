//
// Created by SwiftGan on 2019/4/15.
//

#include <jni.h>
#include <sys/mman.h>
#include <log.h>
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
    do5();
    int a = 1 + 1;
    int b = a + 1;
    int d = a + 1;
    int e = a + 1;
    LOGE("x = %d", 7);
}

void do4replace() {
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
    bool visit(Unit<Base> *unit, void *pc) override {
        Instruction<Base>* instruction = reinterpret_cast<Instruction<Base> *>(unit);
        instruction->assembler();
        return true;
    }
};

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_nativehook_NativeHook_test(JNIEnv *env, jclass jclass1) {


    dosth4Backup = reinterpret_cast<void (*)()>(InlineHook::instance->inlineHook(
            reinterpret_cast<void *>(do4),
            reinterpret_cast<void *>(do4replace)));

//    dosth4Backup2 = reinterpret_cast<void (*)()>(InlineHook::instance->inlineHook(
//            reinterpret_cast<void *>(do4),
//            reinterpret_cast<void *>(do4replace2)));


    do4();

    if (sizeof(void*) == 8) {
        heapPreForkBackup = reinterpret_cast<void (*)(void *)>(SandInlineHookSym("/system/lib64/libart.so", "_ZN3art2gc4Heap13PreZygoteForkEv",
                                                                            reinterpret_cast<void *>(myHeapPreFork)));
        innerSuspendVM = reinterpret_cast<void (*)()>(SandInlineHookSym("/system/lib64/libart.so", "_ZN3art3Dbg9SuspendVMEv",
                                                                        reinterpret_cast<void *>(SuspendVMReplace)));
    } else {
        heapPreForkBackup = reinterpret_cast<void (*)(void *)>(SandInlineHookSym("/system/lib/libart.so", "_ZN3art2gc4Heap13PreZygoteForkEv",
                                                                                 reinterpret_cast<void *>(myHeapPreFork)));
        innerSuspendVM = reinterpret_cast<void (*)()>(SandInlineHookSym("/system/lib/libart.so", "_ZN3art3Dbg9SuspendVMEv",
                                                                        reinterpret_cast<void *>(SuspendVMReplace)));
    }

    void* do3P = reinterpret_cast<void *>(do3);

    InlineHook::instance->breakPoint(reinterpret_cast<void *>(do3), breakCallback);

    LOGE("ok");

    do3(100, 2);


}


extern "C"
EXPORT void* SandInlineHook(void* origin, void* replace) {
    return InlineHook::instance->inlineHook(origin, replace);
}

extern "C"
EXPORT void* SandInlineHookSym(const char* so, const char* symb, void* replace) {
    ElfImg elfImg(so);
    void* origin = reinterpret_cast<void *>(elfImg.getSymbAddress(symb));

    if (origin == nullptr)
        return nullptr;
    return InlineHook::instance->inlineHook(origin, replace);
}