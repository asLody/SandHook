package com.swift.sandhook.nativehook;

/**
 * @author Swift Gan
 * Create: 2019/5/10
 * Desc:
 */
public class NativeHook {


    static {
        System.loadLibrary("sandhook-native");
    }

    public static void dosth() {}

    public static native void test();


}
