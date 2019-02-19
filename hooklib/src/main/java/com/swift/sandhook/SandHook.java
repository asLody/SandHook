package com.swift.sandhook;

import android.os.Build;
import android.util.Log;

import com.swift.sandhook.annotation.HookMode;
import com.swift.sandhook.wrapper.HookErrorException;
import com.swift.sandhook.wrapper.HookWrapper;

import java.lang.reflect.Field;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class SandHook {

    static Map<Member,HookWrapper.HookEntity> globalHookEntityMap = new ConcurrentHashMap<>();
    static Map<Method,HookWrapper.HookEntity> globalBackupMap = new ConcurrentHashMap<>();

    private static HookModeCallBack hookModeCallBack;
    public static void setHookModeCallBack(HookModeCallBack hookModeCallBack) {
        SandHook.hookModeCallBack = hookModeCallBack;
    }

    private static HookResultCallBack hookResultCallBack;
    public static void setHookResultCallBack(HookResultCallBack hookResultCallBack) {
        SandHook.hookResultCallBack = hookResultCallBack;
    }

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

    public static synchronized void hook(HookWrapper.HookEntity entity) throws HookErrorException {

        if (entity == null)
            throw new HookErrorException("null hook entity");

        Member target = entity.target;
        Method hook = entity.hook;
        Method backup = entity.backup;

        if (target == null || hook == null)
            throw new HookErrorException("null input");

        if (globalHookEntityMap.containsKey(entity.target))
            throw new HookErrorException("method <" + entity.target.getName() + "> has been hooked!");

        resolveStaticMethod(target);
        if (backup != null) {
            resolveStaticMethod(backup);
            SandHookMethodResolver.resolveMethod(hook, backup);
        }
        if (target instanceof Method) {
            ((Method)target).setAccessible(true);
        }

        int mode = HookMode.AUTO;
        if (hookModeCallBack != null) {
            mode = hookModeCallBack.hookMode(target);
        }

        int res;
        if (mode != HookMode.AUTO) {
            res = hookMethod(target, hook, backup, mode);
        } else {
            HookMode hookMode = hook.getAnnotation(HookMode.class);
            res = hookMethod(target, hook, backup, hookMode == null ? HookMode.AUTO : hookMode.value());
        }

        if (res > 0 && backup != null) {
            backup.setAccessible(true);
        }

        entity.hookMode = res;

        if (hookResultCallBack != null) {
            hookResultCallBack.hookResult(res > 0, entity);
        }

        if (res < 0) {
            throw new HookErrorException("hook method <" + entity.target.toString() + "> error in native!");
        }

        globalHookEntityMap.put(entity.target, entity);

        if (entity.backup != null) {
            globalBackupMap.put(entity.backup, entity);
        }

        Log.d("SandHook", "method <" + entity.target.toString() + "> hook <" + (res == HookMode.INLINE ? "inline" : "replacement") + "> success!");
    }

    public static Object callOriginMethod(Member originMethod, Object thiz, Object... args) throws Throwable {
        HookWrapper.HookEntity hookEntity = globalHookEntityMap.get(originMethod);
        if (hookEntity == null || hookEntity.backup == null)
            return null;
        return callOriginMethod(originMethod, hookEntity.backup, thiz, args);
    }

    public static Object callOriginByBackup(Method backupMethod, Object thiz, Object... args) throws Throwable {
        HookWrapper.HookEntity hookEntity = globalBackupMap.get(backupMethod);
        if (hookEntity == null)
            return null;
        return callOriginMethod(hookEntity.target, backupMethod, thiz, args);
    }

    public static Object callOriginMethod(Member originMethod, Method backupMethod, Object thiz, Object[] args) throws Throwable {
        backupMethod.setAccessible(true);
        if (Modifier.isStatic(originMethod.getModifiers())) {
            ensureMethodDeclaringClass(originMethod, backupMethod);
            return backupMethod.invoke(null, args);
        } else {
            ensureMethodDeclaringClass(originMethod, backupMethod);
            return backupMethod.invoke(thiz, args);
        }
    }

    public static void ensureBackupDeclaringClass(Method backupMethod) {
        if (backupMethod == null)
            return;
        HookWrapper.HookEntity hookEntity = globalBackupMap.get(backupMethod);
        if (hookEntity == null)
            return;
        ensureMethodDeclaringClass(hookEntity.target, backupMethod);
    }

    public static void ensureBackupDelaringClassByOrigin(Member originMethod) {
        if (originMethod == null)
            return;
        HookWrapper.HookEntity hookEntity = globalHookEntityMap.get(originMethod);
        if (hookEntity == null || hookEntity.backup == null)
            return;
        ensureMethodDeclaringClass(originMethod, hookEntity.backup);
    }



    private static void resolveStaticMethod(Member method) {
        //ignore result, just call to trigger resolve
        try {
            if (method instanceof Method && Modifier.isStatic(method.getModifiers())) {
                ((Method) method).setAccessible(true);
                ((Method) method).invoke(new Object(), getFakeArgs((Method) method));
            }
        } catch (Throwable throwable) {
        }
    }

    private static Object[] getFakeArgs(Method method) {
        Class[] pars = method.getParameterTypes();
        if (pars == null || pars.length == 0) {
            return new Object[]{new Object()};
        } else {
            return null;
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

    public static native void setHookMode(int hookMode);

    //default on!
    public static native void setInlineSafeCheck(boolean check);
    public static native void skipAllSafeCheck(boolean skip);

    private static native int hookMethod(Member originMethod, Method hookMethod, Method backupMethod, int hookMode);

    public static native void ensureMethodCached(Method hook, Method backup);

    public static native void ensureMethodDeclaringClass(Member originMethod, Method backupMethod);

    @FunctionalInterface
    public interface HookModeCallBack {
        int hookMode(Member originMethod);
    }

    @FunctionalInterface
    public interface HookResultCallBack {
        void hookResult(boolean success, HookWrapper.HookEntity hookEntity);
    }

}
