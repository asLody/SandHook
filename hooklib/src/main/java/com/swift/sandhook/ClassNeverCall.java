package com.swift.sandhook;

public class ClassNeverCall {
    private void neverCall() {}
    private static void neverCallStatic() {}
    private native void neverCallNative();
}
