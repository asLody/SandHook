package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.HookMode;
import com.swift.sandhook.annotation.SkipParamCheck;
import com.swift.sandhook.annotation.ThisObject;
import com.swift.sandhook.test.TestClass;

import java.lang.reflect.Method;

@HookClass(TestClass.class)
public class CtrHook {

    @HookMethodBackup
    @SkipParamCheck
    static Method ctrbackup;

    @HookMethodBackup("add1")
    @SkipParamCheck
    static Method add1backup;

    @HookMethodBackup("add2")
    @SkipParamCheck
    static Method add2backup;

    @HookMethod
    public static void onCtr(@ThisObject TestClass thiz, int a) throws Throwable {
        Log.e("TestClassHook", "TestClass(int) been hooked");
        SandHook.callOriginByBackup(ctrbackup, thiz, a);
    }

    @HookMethod("add1")
    @HookMode(HookMode.INLINE)
    public static void onAdd1(TestClass thiz) throws Throwable {
        Log.e("TestClassHook", "add1 been hooked");
        try {
            SandHook.callOriginByBackup(add1backup, thiz);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @HookMethod("add2")
    public static void onAdd2(TestClass thiz) throws  Throwable {
        SandHook.callOriginByBackup(add2backup, thiz);
    }



}
