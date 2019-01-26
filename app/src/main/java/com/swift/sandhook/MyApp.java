package com.swift.sandhook;

import android.app.Application;

import com.swift.sandhook.testHookers.ActivityHooker;
import com.swift.sandhook.testHookers.CtrHook;
import com.swift.sandhook.testHookers.CustmizeHooker;
import com.swift.sandhook.testHookers.JniHooker;
import com.swift.sandhook.testHookers.LogHooker;
import com.swift.sandhook.testHookers.ObjectHooker;
import com.swift.sandhook.wrapper.HookErrorException;

import dalvik.system.DexClassLoader;

public class MyApp extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        try {
            SandHook.addHookClass(JniHooker.class,
                    CtrHook.class,
                    LogHooker.class,
                    CustmizeHooker.class,
                    ActivityHooker.class,
                    ObjectHooker.class);
        } catch (HookErrorException e) {
            e.printStackTrace();
        }

        try {
            ClassLoader classLoader = getClassLoader();
            DexClassLoader dexClassLoader = new DexClassLoader("/sdcard/hookers-debug.apk",
                    getCacheDir().getAbsolutePath(), null, classLoader);
            Class absHookerClass = Class.forName("com.swift.sandhook.hookers.AbsHooker", true, dexClassLoader);
            SandHook.addHookClass(absHookerClass);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (HookErrorException e) {
            e.printStackTrace();
        }

    }
}
