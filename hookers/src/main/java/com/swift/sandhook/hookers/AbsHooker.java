package com.swift.sandhook.hookers;

import android.util.Log;

import com.swift.sandhook.annotation.HookReflectClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;

@HookReflectClass("com.swift.sandhook.test.Inter")
public class AbsHooker {

    @HookMethod("dosth")
    public static void ondosth(Object thiz) {
        Log.e("AbsHooker", "dosth hook success ");
        ondosthBackup(thiz);
    }

    @HookMethodBackup("dosth")
    public static void ondosthBackup(Object thiz) {
        ondosthBackup(thiz);
    }

}
