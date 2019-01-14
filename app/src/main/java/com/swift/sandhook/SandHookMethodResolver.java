package com.swift.sandhook;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class SandHookMethodResolver {

    public static Field resolvedMethodsField;
    public static Field dexCacheField;
    public static Field dexMethodIndexField;
    public static Field artMethodField;

    public static boolean canResolvedInJava = false;

    public static long resolvedMethodsAddress = 0;
    public static int dexMethodIndex = 0;

    public static void init() {
        checkSupport();
    }

    private static void checkSupport() {
        try {
            dexMethodIndexField = SandHook.getField(Method.class, "dexMethodIndex");
            dexMethodIndex = (int) dexMethodIndexField.get(SandHook.testOffsetMethod1);
            dexCacheField = SandHook.getField(Class.class, "dexCache");
            dexCacheField.setAccessible(true);
            Object dexCache = dexCacheField.get(ArtMethodSizeTest.class);
            resolvedMethodsField = SandHook.getField(dexCache.getClass(), "resolvedMethods");
            Object resolvedMethods = resolvedMethodsField.get(dexCache);
            if (resolvedMethods instanceof Long) {
                canResolvedInJava = false;
                resolvedMethodsAddress = (long) resolvedMethods;
            } else if (resolvedMethods instanceof long[]) {
                canResolvedInJava = true;
            } else if (SandHook.hasJavaArtMethod() && resolvedMethods instanceof Object[]) {
                canResolvedInJava = true;
            } else {
                canResolvedInJava = false;
            }
            if (canResolvedInJava) {
                artMethodField = SandHook.getField(Method.class, "artMethod");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void resolveMethod(Method method) {
        if (canResolvedInJava && artMethodField != null) {
            // in java
            try {
                resolveInJava(method);
            } catch (Exception e) {
                // in native
            }
        } else {
            // in native
        }
    }

    private static void resolveInJava(Method method) throws Exception {
        Object dexCache = dexCacheField.get(method.getDeclaringClass());
        resolvedMethodsField.setAccessible(true);
        int dexMethodIndex = (int) dexMethodIndexField.get(method);
        Object resolvedMethods = resolvedMethodsField.get(dexCache);

        if (resolvedMethods instanceof long[]) {
            long artMethod = (long) artMethodField.get(method);
            ((long[])resolvedMethods)[dexMethodIndex] = artMethod;
        } else if (resolvedMethods instanceof Object[]) {
            Object artMethod = artMethodField.get(method);
            ((Object[])resolvedMethods)[dexMethodIndex] = artMethod;
        } else {
            throw new UnsupportedOperationException("unsupport");
        }
    }

}
