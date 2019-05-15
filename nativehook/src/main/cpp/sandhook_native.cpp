//
// Created by SwiftGan on 2019/4/15.
//

#include <jni.h>
#include <sys/mman.h>
#include "sandhook_native.h"
#include "inst_arm64.h"
#include "decoder_arm64.h"
#include "hook.h"
#include "log.h"

using namespace SandHook::Asm;
using namespace SandHook::Decoder;
using namespace SandHook::Hook;

void (*dosth3Backup)(int, int) = nullptr;
void (*dosth4Backup)() = nullptr;


bool memUnprotect(Addr addr, Addr len) {
    long pagesize = 4096;
    unsigned alignment = (unsigned)((unsigned long long)addr % pagesize);
    int i = mprotect((void *) (addr - alignment), (size_t) (alignment + len),
                     PROT_READ | PROT_WRITE | PROT_EXEC);
    if (i == -1) {
        return false;
    }
    return true;
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
    LOGE("x = %d", x);
}

void do5() {
    LOGE("x = %d", 5);
}

void do4() {
    do5();
    LOGE("x = %d", 6);
    LOGE("x = %d", 7);
}

void do4replace() {
    int a = 1 + 1;
    int b = 1 + 1;
    int c = 1 + 1;
    int d = 1 + 1;
    dosth4Backup();
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

class Visitor : public InstVisitor {
    bool visit(Unit<Base> *unit, void *pc) override {
        return true;
    }
};

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_nativehook_NativeHook_test(JNIEnv *env, jclass jclass1) {

    union {
        InstA64 raw = 0xF9001043;
        STRUCT_A64(STR_UIMM) str;
    } test;

    InstA64* codebl = reinterpret_cast<InstA64 *>((Addr)do1 + 8);

    if (IS_OPCODE(*codebl, B_BL)) {

        //decode
        A64_B_BL a64bl(*reinterpret_cast<STRUCT_A64(B_BL)*>(codebl));
        Off off = a64bl.offset;
        void (*dosth2)() =reinterpret_cast<void (*)()>(a64bl.getImmPCOffsetTarget());
        dosth2();

        //asm
        memUnprotect(reinterpret_cast<Addr>(a64bl.get()), a64bl.size());
        a64bl.assembler();
        Off off1 = a64bl.getImmPCOffset();

        do1();

    }

    if (IS_OPCODE(test.raw, STR_UIMM)) {
        A64_STR_UIMM str(test.str);
        str.assembler();
        str.get();
    }

    Arm64Decoder arm64Decoder = Arm64Decoder();

    Visitor visitor = Visitor();

    arm64Decoder.decode(reinterpret_cast<void *>(do1), 4 * 8, visitor);

    InlineHookArm64Android inlineHookArm64Android = InlineHookArm64Android();

    do4();

    dosth4Backup = reinterpret_cast<void (*)()>(inlineHookArm64Android.inlineHook(
            reinterpret_cast<void *>(do4),
            reinterpret_cast<void *>(do4replace)));

    do4();

}