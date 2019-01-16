package com.swift.sandhook;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class SandHook {

    public static Class artMethodClass;

    public static Method testOffsetMethod1;
    public static Method testOffsetMethod2;
    public static Object testOffsetArtMethod1;
    public static Object testOffsetArtMethod2;

    public static int testAccessFlag;


    public static boolean init() {
        initTestOffset();
        SandHookMethodResolver.init();
        return true;
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


}
