package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.HookMode;
import com.swift.sandhook.wrapper.HookWrapper;

@HookClass(Object.class)
public class ObjectHooker {

    @HookMethodBackup("toString")
    static HookWrapper.HookEntity toStrHook;

    @HookMethod("toString")
    @HookMode(HookMode.INLINE)
    public static String toStr(Object thiz) throws Throwable {
        Log.e("ObjectHooker", "hooked success ");
        return (String) toStrHook.callOrigin(thiz);
    }

}
