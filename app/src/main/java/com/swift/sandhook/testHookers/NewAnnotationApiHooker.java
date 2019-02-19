package com.swift.sandhook.testHookers;

import android.app.Activity;
import android.util.Log;

import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.Param;
import com.swift.sandhook.annotation.ThisObject;
import com.swift.sandhook.test.TestClass;

@HookClass(TestClass.class)
public class NewAnnotationApiHooker {

    @HookMethod("testNewHookApi")
    public static void onTestNewHookApi(@ThisObject TestClass thiz, @Param("com.swift.sandhook.MainActivity") Activity activity, int a) {
        Log.e("TestClassHook", "testNewHookApi been hooked");
        onTestNewHookApiBackup(thiz, activity, a);
    }

    @HookMethodBackup("testNewHookApi")
    public static void onTestNewHookApiBackup(@ThisObject TestClass thiz, @Param("com.swift.sandhook.MainActivity") Activity activity, int a) {
        onTestNewHookApiBackup(thiz, activity, a);
    }

}
