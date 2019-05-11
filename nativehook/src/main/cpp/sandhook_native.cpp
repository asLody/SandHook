//
// Created by SwiftGan on 2019/4/15.
//

#include <jni.h>
#include <sys/mman.h>
#include "sandhook_native.h"
#include "inst_arm64.h"

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
}

void do1() {
    do2();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_nativehook_NativeHook_test(JNIEnv *env, jclass jclass1) {

    union {
        InstA64 raw = 0x36A01005;
        STRUCT_A64(TBZ_TBNZ) bl;
    } test;

    InstA64* codebl = reinterpret_cast<InstA64 *>((Addr)do1 + 8);

    if (IS_OPCODE(*codebl, B_BL)) {

        //decode
        A64_B_BL a64bl(reinterpret_cast<STRUCT_A64(B_BL)*>(codebl));
        Off off = a64bl.offset;
        void (*dosth2)() =reinterpret_cast<void (*)()>(a64bl.getImmPCOffsetTarget());
        dosth2();

        //asm
        memUnprotect(reinterpret_cast<Addr>(a64bl.get()), a64bl.size());
        a64bl.assembler();
        Off off1 = a64bl.getImmPCOffset();

        do1();

    }

}