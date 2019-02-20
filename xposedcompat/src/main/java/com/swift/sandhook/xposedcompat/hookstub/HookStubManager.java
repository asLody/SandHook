package com.swift.sandhook.xposedcompat.hookstub;

import java.lang.reflect.Member;

public class HookStubManager {

    public final static int MAX_STUB_COUNT = 100;

    public static Member[] originMethods = new Member[MAX_STUB_COUNT];


    public static String getHookMethodName(int index) {
        return "stub_hook_" + index;
    }

    public static String getbackupMethodName(int index) {
        return "stub_backup_" + index;
    }

    public static Object hookBridge(int index, CallOriginCallBack callOrigin, long... stubArgs) throws Throwable {
        Member originMethod = originMethods[index];
        return null;
    }

}
