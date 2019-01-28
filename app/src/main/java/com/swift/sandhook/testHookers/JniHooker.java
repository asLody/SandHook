package com.swift.sandhook.testHookers;

import android.util.Log;

import com.swift.sandhook.test.TestClass;
import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;

@HookClass(TestClass.class)
public class JniHooker {

    @HookMethod("jni_test")
    public static int onJni(TestClass thiz) {
        Log.e("JniHooker", "hooked success ");
        return onJniBackup(thiz);
    }

    @HookMethodBackup("jni_test")
    public static int onJniBackup(TestClass thiz) {
        return onJniBackup(thiz);
    }

}
