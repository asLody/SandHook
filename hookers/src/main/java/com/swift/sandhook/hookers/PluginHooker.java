package com.swift.sandhook.hookers;

import android.util.Log;

import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.HookReflectClass;
import com.swift.sandhook.annotation.MethodReflectParams;

import java.lang.reflect.Method;

@HookReflectClass("com.swift.sandhook.MainActivity")
public class PluginHooker {

    @HookMethodBackup("testPluginHook")
    @MethodReflectParams("com.swift.sandhook.test.TestClass")
    static Method backup;

    @HookMethod("testPluginHook")
    @MethodReflectParams("com.swift.sandhook.test.TestClass")
    public static int testPluginHook(Object thiz, Object test) {
        Log.e("PluginHooker", "testPluginHook be hooked");
        return testPluginHookBackup(thiz, test);
    }

    @HookMethodBackup("testPluginHook")
    @MethodReflectParams("com.swift.sandhook.test.TestClass")
    public static int testPluginHookBackup(Object thiz, Object test) {
        return testPluginHookBackup(thiz, test);
    }

}
