package com.swift.sandhook.xposedcompat.methodgen;

import android.os.Trace;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.wrapper.HookWrapper;
import com.swift.sandhook.xposedcompat.XposedCompat;
import com.swift.sandhook.xposedcompat.hookstub.HookMethodEntity;
import com.swift.sandhook.xposedcompat.hookstub.HookStubManager;
import com.swift.sandhook.xposedcompat.utils.DexLog;
import com.swift.sandhook.xposedcompat.utils.FileUtils;

import java.io.File;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.concurrent.atomic.AtomicBoolean;

import de.robv.android.xposed.XposedBridge;

public final class DynamicBridge {

    private static final HashMap<Member, Method> hookedInfo = new HashMap<>();
    private static final HookerDexMaker dexMaker = new HookerDexMaker();
    private static final AtomicBoolean dexPathInited = new AtomicBoolean(false);
    private static File dexDir;

    public static void onForkPost() {
        dexPathInited.set(false);
    }

    public static synchronized void hookMethod(Member hookMethod, XposedBridge.AdditionalHookInfo additionalHookInfo) {

        if (!checkMember(hookMethod)) {
            return;
        }

        if (hookedInfo.containsKey(hookMethod)) {
            DexLog.w("already hook method:" + hookMethod.toString());
            return;
        }

        DexLog.d("start to generate class for: " + hookMethod);
        try {
            // using file based DexClassLoader
            if (dexPathInited.compareAndSet(false, true)) {
                // delete previous compiled dex to prevent potential crashing
                // TODO find a way to reuse them in consideration of performance
                try {
                    String fixedAppDataDir = XposedCompat.cacheDir.getAbsolutePath();
                    dexDir = new File(fixedAppDataDir, "/sandxposed/");
                    if (!dexDir.exists())
                        dexDir.mkdirs();
                } catch (Throwable throwable) {
                    DexLog.e("error when init dex path", throwable);
                }
            }
            Trace.beginSection("SandHook-Xposed");
            long timeStart = System.currentTimeMillis();
            HookMethodEntity stub = HookStubManager.getHookMethodEntity(hookMethod);
            if (stub != null) {
                SandHook.hook(new HookWrapper.HookEntity(hookMethod, stub.hook, stub.backup));
            } else {
                dexMaker.start(hookMethod, additionalHookInfo,
                        XposedCompat.classLoader, dexDir == null ? null : dexDir.getAbsolutePath());
                hookedInfo.put(hookMethod, dexMaker.getCallBackupMethod());
            }
            DexLog.d("hook method <" + hookMethod.toString() + "> use " + (System.currentTimeMillis() - timeStart) + " ms.");
            Trace.endSection();
        } catch (Exception e) {
            DexLog.e("error occur when generating dex. dexDir=" + dexDir, e);
        }
    }

    public static void clearOatFile() {
        String fixedAppDataDir = XposedCompat.cacheDir.getAbsolutePath();
        File dexOatDir = new File(fixedAppDataDir, "/sandxposed/oat/");
        if (!dexOatDir.exists())
            return;
        try {
            FileUtils.delete(dexOatDir);
            dexOatDir.mkdirs();
        } catch (Throwable throwable) {
        }
    }

    private static boolean checkMember(Member member) {

        if (member instanceof Method) {
            return true;
        } else if (member instanceof Constructor<?>) {
            return true;
        } else if (member.getDeclaringClass().isInterface()) {
            DexLog.e("Cannot hook interfaces: " + member.toString());
            return false;
        } else if (Modifier.isAbstract(member.getModifiers())) {
            DexLog.e("Cannot hook abstract methods: " + member.toString());
            return false;
        } else {
            DexLog.e("Only methods and constructors can be hooked: " + member.toString());
            return false;
        }
    }

    public static Object invokeOriginalMethod(Member method, Object thisObject, Object[] args)
            throws InvocationTargetException, IllegalAccessException {
        Method callBackup = hookedInfo.get(method);
        if (callBackup == null) {
            throw new IllegalStateException("method not hooked, cannot call original method.");
        }
        if (!Modifier.isStatic(callBackup.getModifiers())) {
            throw new IllegalStateException("original method is not static, something must be wrong!");
        }
        callBackup.setAccessible(true);
        if (args == null) {
            args = new Object[0];
        }
        final int argsSize = args.length;
        if (Modifier.isStatic(method.getModifiers())) {
            return callBackup.invoke(null, args);
        } else {
            Object[] newArgs = new Object[argsSize + 1];
            newArgs[0] = thisObject;
            for (int i = 1; i < newArgs.length; i++) {
                newArgs[i] = args[i - 1];
            }
            return callBackup.invoke(null, newArgs);
        }
    }
}


