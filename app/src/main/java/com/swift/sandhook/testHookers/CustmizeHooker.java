package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.MainActivity;
import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.MethodParams;

import java.lang.reflect.Method;

@HookClass(MainActivity.class)
public class CustmizeHooker {

    @HookMethodBackup("methodBeHooked")
    @MethodParams({int.class, int.class})
    static Method backup;

    @HookMethod("methodBeHooked")
    @MethodParams({int.class, int.class})
    public static int staticMethodHooked(int a, int b) {
        Log.e("CustmizeHooker", "methodBeHooked be hooked");
        try {
            return (int) backup.invoke(null, a, b);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return 0;
    }

    @HookMethodBackup("methodBeHooked")
    @MethodParams({int.class, int.class})
    public static int staticMethodHookedBackup(int a, int b) {
        return 0;
    }

}
