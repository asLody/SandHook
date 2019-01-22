package com.swift.sandhook.wrapper;

import android.util.Log;

@HookClass(Log.class)
public class LogHooker {

    @HookMethod("w")
    @MethodParams({String.class, String.class})
    public static int onCreate(String tag, String msg) {
        Log.e("LogHooker", "hooked success " + tag);
        return onCreateBackup(tag, msg);
    }

    @HookMethodBackup("w")
    @MethodParams({String.class, String.class})
    public static int onCreateBackup(String tag, String msg) {
        return 0;
    }

}
