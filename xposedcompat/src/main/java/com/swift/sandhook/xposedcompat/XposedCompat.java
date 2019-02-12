package com.swift.sandhook.xposedcompat;

import android.content.Context;
import android.content.pm.ApplicationInfo;

import java.io.File;

public class XposedCompat {
    public static File cacheDir;
    public static Context context;
    public static ClassLoader classLoader;
    public static boolean isFirstApplication;
}
