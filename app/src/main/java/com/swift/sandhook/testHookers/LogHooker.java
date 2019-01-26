package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.MethodParams;

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
