package com.swift.sandhook;

import android.os.Build;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.swift.sandhook.wrapper.HookErrorException;
import com.swift.sandhook.wrapper.HookWrapper;

import java.lang.reflect.Field;
import java.lang.reflect.Member;
import java.lang.reflect.Method;

public class SandHook {

    public static Class artMethodClass;

    public static Field nativePeerField;
    public static Method testOffsetMethod1;
    public static Method testOffsetMethod2;
    public static Object testOffsetArtMethod1;
    public static Object testOffsetArtMethod2;

    public static int testAccessFlag;

    static {
        System.loadLibrary("native-lib");
        init();
    }

    private static boolean init() {
        initTestOffset();
        initThreadPeer();
        SandHookMethodResolver.init();
        return initNative(Build.VERSION.SDK_INT);
    }

    private static void initThreadPeer() {
        try {
            nativePeerField = getField(Thread.class, "nativePeer");
        } catch (NoSuchFieldException e) {

        }
    }

    public static void addHookClass(Class... hookWrapperClass) throws HookErrorException {
        HookWrapper.addHookClass(hookWrapperClass);
    }

    public static void hook(@NonNull Member target, @NonNull Method hook, @Nullable Method backup) {
        if (backup != null) {
            resolveBackupMethod(backup);
            SandHookMethodResolver.resolveMethod(hook, backup);
        }
        hookMethod(target, hook, backup);
    }

    private static void resolveBackupMethod(Method method) {
        //ignore result, just call to trigger resolve
        try {
            method.invoke(null, null);
        } catch (Exception e) {
        }
    }

    private static void initTestOffset() {
        // make test methods sure resolved!
        ArtMethodSizeTest.method1();
        ArtMethodSizeTest.method2();
        // get test methods
        try {
            testOffsetMethod1 = ArtMethodSizeTest.class.getDeclaredMethod("method1");
            testOffsetMethod2 = ArtMethodSizeTest.class.getDeclaredMethod("method2");
        } catch (NoSuchMethodException e) {
            throw new RuntimeException("SandHook init error", e);
        }
        initTestAccessFlag();
    }

    private static void initTestAccessFlag() {
        if (hasJavaArtMethod()) {
            try {
                loadArtMethod();
                Field fieldAccessFlags = getField(artMethodClass, "accessFlags");
                testAccessFlag = (int) fieldAccessFlags.get(testOffsetArtMethod1);
            } catch (Exception e) {
            }
        } else {
            try {
                Field fieldAccessFlags = getField(Method.class, "accessFlags");
                testAccessFlag = (int) fieldAccessFlags.get(testOffsetMethod1);
            } catch (Exception e) {
            }
        }
    }

    private static void loadArtMethod() {
        try {
            Field fieldArtMethod = getField(Method.class, "artMethod");
            testOffsetArtMethod1 = fieldArtMethod.get(testOffsetMethod1);
            testOffsetArtMethod2 = fieldArtMethod.get(testOffsetMethod2);
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }


    public static boolean hasJavaArtMethod() {
        if (artMethodClass != null)
            return true;
        try {
            artMethodClass = Class.forName("java.lang.reflect.ArtMethod");
            return true;
        } catch (ClassNotFoundException e) {
            return false;
        }
    }

    public static Field getField(Class topClass, String fieldName) throws NoSuchFieldException {
        while (topClass != null && topClass != Object.class) {
            try {
                Field field = topClass.getDeclaredField(fieldName);
                field.setAccessible(true);
                return field;
            } catch (Exception e) {
            }
            topClass = topClass.getSuperclass();
        }
        throw new NoSuchFieldException(fieldName);
    }

    public static long getThreadId() {
        if (nativePeerField == null)
            return 0;
        try {
            return (long) nativePeerField.get(Thread.currentThread());
        } catch (IllegalAccessException e) {
            return 0;
        }
    }

    private static native boolean initNative(int sdk);

    private static native boolean hookMethod(Member originMethod, Method hookMethod, Method backupMethod);

    public static native void ensureMethodCached(Method hook, Method backup);

}
