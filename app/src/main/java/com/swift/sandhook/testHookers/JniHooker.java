package com.swift.sandhook.testHookers;

import android.os.Build;
import android.util.Log;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.SkipParamCheck;
import com.swift.sandhook.annotation.ThisObject;
import com.swift.sandhook.test.TestClass;

import java.lang.reflect.Method;

@HookClass(TestClass.class)
public class JniHooker {

    @HookMethodBackup("jni_test")
    @SkipParamCheck
    static Method backup;

    @HookMethod("jni_test")
    public static void onJni(@ThisObject TestClass thiz) throws Throwable {
        Log.e("JniHooker", "hooked success ");
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.KITKAT) {
            SandHook.callOriginByBackup(backup, thiz);
        }
    }

}
