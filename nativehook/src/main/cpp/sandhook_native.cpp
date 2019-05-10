//
// Created by SwiftGan on 2019/4/15.
//

#include <jni.h>
#include "sandhook_native.h"
#include "inst_arm64.h"


void do2() {

}

void do1() {
    do2();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_swift_sandhook_nativehook_NativeHook_test(JNIEnv *env, jclass jclass1) {

    do1();

    U8 s = sizeof(STRUCT_A64(B_BL));

    union {
        InstA64 raw = 0x36A01005;
        STRUCT_A64(TBZ_TBNZ) bl;
    } test;

    if (IS_OPCODE(test.raw, TBZ_TBNZ)) {

        STRUCT_A64(TBZ_TBNZ) bl = test.bl;

        A64_TBZ_TBNZ a64ldr(bl);

        Off off = a64ldr.offset;
    }

}