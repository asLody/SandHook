package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.Param;
import com.swift.sandhook.annotation.SkipParamCheck;

import java.lang.reflect.Method;

@HookClass(Log.class)
public class LogHooker {

    @HookMethodBackup("w")
    @SkipParamCheck
    static Method backup;

    @HookMethod("w")
    public static int onCreate(String tag, @Param("java.lang.String") Object msg) throws Throwable {
        Log.e("LogHooker", "hooked success " + tag);
        return (int) SandHook.callOriginByBackup(backup, null, tag, msg);
    }

}
