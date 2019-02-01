package com.swift.sandhook;

import android.os.Build;

import com.swift.sandhook.annotation.HookMode;
import com.swift.sandhook.wrapper.HookErrorException;
import com.swift.sandhook.wrapper.HookWrapper;

import java.lang.reflect.Field;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

public class SandHook {

    public static Class artMethodClass;

    public static Field nativePeerField;
    public static Method testOffsetMethod1;
    public static Method testOffsetMethod2;
    public static Object testOffsetArtMethod1;
    public static Object testOffsetArtMethod2;

    public static int testAccessFlag;

    static {
        if (SandHookConfig.libSandHookPath == null || SandHookConfig.libSandHookPath.length() == 0) {
            System.loadLibrary("sandhook");
        } else {
            System.load(SandHookConfig.libSandHookPath);
        }
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

    public static void addHookClass(ClassLoader classLoader, Class... hookWrapperClass) throws HookErrorException {
        HookWrapper.addHookClass(classLoader, hookWrapperClass);
    }

    public static boolean hook(Member target, Method hook, Method backup) {
        if (target == null || hook == null)
            return false;
        resolveStaticMethod(target);
        if (backup != null) {
            resolveStaticMethod(backup);
            SandHookMethodResolver.resolveMethod(hook, backup);
        }
        if (target instanceof Method) {
            ((Method)target).setAccessible(true);
        }
        HookMode hookMode = hook.getAnnotation(HookMode.class);
        boolean res = hookMethod(target, hook, backup, hookMode == null ? HookMode.AUTO : hookMode.value());
        if (res && backup != null) {
            backup.setAccessible(true);
        }
        return res;
    }

    private static void resolveStaticMethod(Member method) {
        //ignore result, just call to trigger resolve
        try {
            if (method instanceof Method && Modifier.isStatic(method.getModifiers())) {
                ((Method) method).setAccessible(true);
                ((Method) method).invoke(new Object());
            }
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

    private static native boolean hookMethod(Member originMethod, Method hookMethod, Method backupMethod, int hookMode);

    public static native void ensureMethodCached(Method hook, Method backup);

}
