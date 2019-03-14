package com.swift.sandhook;

import android.os.Build;

import com.swift.sandhook.lib.BuildConfig;

public class SandHookConfig {
    public volatile static int SDK_INT = Build.VERSION.SDK_INT;
    public volatile static String libSandHookPath;
    public volatile static boolean DEBUG = BuildConfig.DEBUG;
}
