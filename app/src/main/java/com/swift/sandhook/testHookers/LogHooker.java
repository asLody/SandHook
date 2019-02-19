package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.HookMode;
import com.swift.sandhook.annotation.Param;

@HookClass(Log.class)
public class LogHooker {

    @HookMethod("w")
    @HookMode(HookMode.INLINE)
    public static int onCreate(String tag, @Param("java.lang.String") Object msg) {
        Log.e("LogHooker", "hooked success " + tag);
        return onCreateBackup(tag, msg);
    }

    @HookMethodBackup("w")
    public static int onCreateBackup(String tag, @Param("java.lang.String") Object msg) {
        return onCreateBackup(tag, msg);
    }

}
