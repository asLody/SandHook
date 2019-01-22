package com.swift.sandhook;

import android.app.Application;

import com.swift.sandhook.wrapper.HookErrorException;
import com.swift.sandhook.wrapper.HookWrapper;

public class MyApp extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        ActivityHooker.onCreate(null, null);
        try {
            HookWrapper.addHookClass(ActivityHooker.class);
        } catch (HookErrorException e) {
            e.printStackTrace();
        }
    }
}
