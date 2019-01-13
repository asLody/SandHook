package com.swift.sandhook;

public class SandHook {
    public final static boolean init() {
        // make sure resolved!
        ArtMethodSizeTest.method1();
        ArtMethodSizeTest.method2();
        return true;
    }
}
