package com.swift.sandhook;

import android.os.Bundle;
import android.util.Log;

import com.swift.sandhook.wrapper.HookClass;
import com.swift.sandhook.wrapper.HookMethod;
import com.swift.sandhook.wrapper.HookMethodBackup;
import com.swift.sandhook.wrapper.MethodParams;

@HookClass(MainActivity.class)
public class ActivityHooker {

    @HookMethod("methodBeHooked")
    @MethodParams(Bundle.class)
    public static void onCreate(MainActivity thiz, Bundle bundle) {
        Log.e("ActivityHooker", "hooked success " + thiz);
        onCreateBackup(thiz, bundle);
    }

    @HookMethodBackup("methodBeHooked")
    @MethodParams(Bundle.class)
    public static void onCreateBackup(MainActivity thiz, Bundle bundle) {

    }

}
