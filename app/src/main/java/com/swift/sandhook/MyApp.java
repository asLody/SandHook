package com.swift.sandhook;

import android.app.Application;

import com.swift.sandhook.wrapper.HookErrorException;
import com.swift.sandhook.wrapper.LogHooker;

public class MyApp extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        try {
            SandHook.addHookClass(LogHooker.class, ActivityHooker.class);
        } catch (HookErrorException e) {
            e.printStackTrace();
        }
    }
}
