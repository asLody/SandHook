package com.swift.sandhook.test;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.swift.sandhook.wrapper.HookClass;
import com.swift.sandhook.wrapper.HookMethod;
import com.swift.sandhook.wrapper.HookMethodBackup;
import com.swift.sandhook.wrapper.MethodParams;

@HookClass(Activity.class)
public class ActivityHooker {

    @HookMethod("onCreate")
    @MethodParams(Bundle.class)
    public static void onCreate(Activity thiz, Bundle bundle) {
        Log.e("ActivityHooker", "hooked onCreate success " + thiz);
        onCreateBackup(thiz, bundle);
    }

    @HookMethodBackup("onCreate")
    @MethodParams(Bundle.class)
    public static void onCreateBackup(Activity thiz, Bundle bundle) {

    }

    @HookMethod("onPause")
    public static void onPause(Activity thiz) {
        Log.e("ActivityHooker", "hooked onPause success " + thiz);
        onPauseBackup(thiz);
    }

    @HookMethodBackup("onPause")
    public static void onPauseBackup(Activity thiz) {

    }

}
