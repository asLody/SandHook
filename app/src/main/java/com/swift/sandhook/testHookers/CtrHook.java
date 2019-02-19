package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.HookMode;
import com.swift.sandhook.annotation.ThisObject;
import com.swift.sandhook.test.TestClass;

@HookClass(TestClass.class)
public class CtrHook {

    @HookMethod
    public static void onCtr(@ThisObject TestClass thiz, int a) {
        Log.e("TestClassHook", "TestClass(int) been hooked");
        onCtrBackup(thiz, a);
    }

    @HookMethodBackup
    public static void onCtrBackup(@ThisObject TestClass thiz, int a) {
        onCtrBackup(thiz, a);
    }

    @HookMethod("add1")
    @HookMode(HookMode.INLINE)
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
