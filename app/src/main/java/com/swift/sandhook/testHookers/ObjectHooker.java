package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.wrapper.HookClass;
import com.swift.sandhook.wrapper.HookMethod;
import com.swift.sandhook.wrapper.HookMethodBackup;

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
