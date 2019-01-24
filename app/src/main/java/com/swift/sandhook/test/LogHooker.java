package com.swift.sandhook.test;

import android.util.Log;

import com.swift.sandhook.wrapper.HookClass;
import com.swift.sandhook.wrapper.HookMethod;
import com.swift.sandhook.wrapper.HookMethodBackup;
import com.swift.sandhook.wrapper.MethodParams;

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
        return onCreateBackup(tag, msg);
    }

}
