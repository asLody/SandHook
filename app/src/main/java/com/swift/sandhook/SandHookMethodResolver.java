package com.swift.sandhook;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

import static com.swift.sandhook.SandHook.artMethodClass;
import static com.swift.sandhook.SandHook.getField;
import static com.swift.sandhook.SandHook.hasJavaArtMethod;

public class SandHookMethodResolver {

    public static Field resolvedMethodsField;
    public static Field dexCacheField;
    public static Field dexMethodIndexField;
    public static Field artMethodField;

    public static boolean canResolvedInJava = false;
    public static boolean isArtMethod = false;

    public static long resolvedMethodsAddress = 0;
    public static int dexMethodIndex = 0;

    public static Method testMethod;
    public static Object testArtMethod;

    public static void init() {
        testMethod = SandHook.testOffsetMethod1;
        checkSupport();
    }

    private static void checkSupport() {
        try {
            artMethodField = getField(Method.class, "artMethod");

            testArtMethod = artMethodField.get(testMethod);

            if (hasJavaArtMethod() && testArtMethod.getClass() == artMethodClass) {
                checkSupportForArtMethod();
                isArtMethod = true;
            } else if (testArtMethod instanceof Long) {
                checkSupportForArtMethodId();
                isArtMethod = false;
            } else {
                canResolvedInJava = false;
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // may 5.0
    private static void checkSupportForArtMethod() throws Exception {
        dexMethodIndexField = getField(artMethodClass, "dexMethodIndex");
        dexCacheField = getField(Class.class, "dexCache");
        Object dexCache = dexCacheField.get(testMethod.getDeclaringClass());
        resolvedMethodsField = getField(dexCache.getClass(), "resolvedMethods");
        if (resolvedMethodsField.get(dexCache) instanceof Object[]) {
            canResolvedInJava = true;
        }
    }

    // may 6.0
    private static void checkSupportForArtMethodId() throws Exception {
        dexMethodIndexField = getField(Method.class, "dexMethodIndex");
        dexMethodIndex = (int) dexMethodIndexField.get(testMethod);
        dexCacheField = getField(Class.class, "dexCache");
        Object dexCache = dexCacheField.get(testMethod.getDeclaringClass());
        resolvedMethodsField = getField(dexCache.getClass(), "resolvedMethods");
        Object resolvedMethods = resolvedMethodsField.get(dexCache);
        if (resolvedMethods instanceof Long) {
            canResolvedInJava = false;
            resolvedMethodsAddress = (long) resolvedMethods;
        } else if (resolvedMethods instanceof long[]) {
            canResolvedInJava = true;
        }
    }

    public static void resolveMethod(Method hook, Method backup) {
        if (canResolvedInJava && artMethodField != null) {
            // in java
            try {
                resolveInJava(hook, backup);
            } catch (Exception e) {
                // in native
                resolveInNative(hook, backup);
            }
        } else {
            // in native
            resolveInNative(hook, backup);
        }
    }

    private static void resolveInJava(Method hook, Method backup) throws Exception {
        Object dexCache = dexCacheField.get(hook.getDeclaringClass());
        if (isArtMethod) {
            Object artMethod = artMethodField.get(backup);
            int dexMethodIndex = (int) dexMethodIndexField.get(artMethod);
            Object resolvedMethods = resolvedMethodsField.get(dexCache);
            ((Object[])resolvedMethods)[dexMethodIndex] = artMethod;
        } else {
            int dexMethodIndex = (int) dexMethodIndexField.get(backup);
            Object resolvedMethods = resolvedMethodsField.get(dexCache);
            long artMethod = (long) artMethodField.get(backup);
            ((long[])resolvedMethods)[dexMethodIndex] = artMethod;
        }
    }

    private static void resolveInNative(Method hook, Method backup) {
//        HookMain.ensureMethodCached(hook, backup);
    }

}
