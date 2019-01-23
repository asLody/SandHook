package com.swift.sandhook.test;

import android.util.Log;

import com.swift.sandhook.MainActivity;
import com.swift.sandhook.wrapper.HookClass;
import com.swift.sandhook.wrapper.HookMethod;
import com.swift.sandhook.wrapper.MethodParams;

@HookClass(MainActivity.class)
public class CustmizeHooker {

    @HookMethod("methodBeHooked")
    @MethodParams({int.class, int.class})
    public static int staticMethodHooked(int a, int b) {
        Log.e("CustmizeHooker", "methodBeHooked be hooked");
        return 0;
    }

}
