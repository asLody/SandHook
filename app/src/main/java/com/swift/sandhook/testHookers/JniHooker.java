package com.swift.sandhook.testHookers;

import android.os.Build;
import android.util.Log;

import com.swift.sandhook.annotation.HookMode;
import com.swift.sandhook.annotation.ThisObject;
import com.swift.sandhook.test.TestClass;
import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;

@HookClass(TestClass.class)
public class JniHooker {

    @HookMethod("jni_test")
    @HookMode(HookMode.INLINE)
    public static int onJni(@ThisObject TestClass thiz) {
        Log.e("JniHooker", "hooked success ");
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.KITKAT) {
            return onJniBackup(thiz);
        } else {
            //can not call jni origin in 4.4 tmp
            return 0;
        }
    }

    @HookMethodBackup("jni_test")
    public static int onJniBackup(@ThisObject TestClass thiz) {
        return onJniBackup(thiz);
    }

}
