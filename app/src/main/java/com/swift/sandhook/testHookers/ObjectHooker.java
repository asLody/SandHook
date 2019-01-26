package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;

@HookClass(Object.class)
public class ObjectHooker {

    @HookMethod("toString")
    public static String toStr(Object thiz) {
        Log.e("ObjectHooker", "hooked success ");
        return toStrBackup(thiz);
    }

    @HookMethodBackup("toString")
    public static String toStrBackup(Object thiz) {
        return toStrBackup(thiz);
    }

}
