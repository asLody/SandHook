package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.wrapper.HookClass;
import com.swift.sandhook.wrapper.HookMethod;
import com.swift.sandhook.wrapper.HookMethodBackup;

@HookClass(Runnable.class)
public class AbsHooker {

    @HookMethod("run")
    public static void onRun(Object thiz) {
        Log.e("AbsHooker", "Runnable hook success ");
        onRunBackup(thiz);
    }

    @HookMethodBackup("run")
    public static void onRunBackup(Object thiz) {
        onRunBackup(thiz);
    }

}
