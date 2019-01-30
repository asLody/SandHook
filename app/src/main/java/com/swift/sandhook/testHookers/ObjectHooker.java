package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.HookMode;

@HookClass(Object.class)
public class ObjectHooker {

    @HookMethod("toString")
    @HookMode(HookMode.INLINE)
    public static String toStr(Object thiz) {
        Log.e("ObjectHooker", "hooked success ");
        return toStrBackup(thiz);
    }

    @HookMethodBackup("toString")
    public static String toStrBackup(Object thiz) {
        return toStrBackup(thiz);
    }

}
