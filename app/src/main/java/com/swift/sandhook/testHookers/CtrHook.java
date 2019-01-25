package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.test.TestClass;
import com.swift.sandhook.wrapper.HookClass;
import com.swift.sandhook.wrapper.HookMethod;
import com.swift.sandhook.wrapper.HookMethodBackup;
import com.swift.sandhook.wrapper.MethodParams;

@HookClass(TestClass.class)
public class CtrHook {

    @HookMethod
    @MethodParams(int.class)
    public static void onCtr(TestClass thiz, int a) {
        Log.e("TestClassHook", "TestClass(int) been hooked");
        onCtrBackup(thiz, a);
    }

    @HookMethodBackup
    @MethodParams(int.class)
    public static void onCtrBackup(TestClass thiz, int a) {
        onCtrBackup(thiz, a);
    }

    @HookMethod("add1")
    public static void onAdd1(TestClass thiz) {
        Log.e("TestClassHook", "add1 been hooked");
        try {
            onAdd1Backup(thiz);
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    @HookMethodBackup("add1")
    public static void onAdd1Backup(TestClass thiz) {
        onAdd1Backup(thiz);
    }

    @HookMethod("add2")
    public static void onAdd2(TestClass thiz) {
        Log.e("TestClassHook", "add2 been hooked");
        onAdd2Backup(thiz);
    }

    @HookMethodBackup("add2")
    public static void onAdd2Backup(TestClass thiz) {
        onAdd2Backup(thiz);
    }

}
