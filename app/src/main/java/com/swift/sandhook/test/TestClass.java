package com.swift.sandhook.test;

public class TestClass {

    public int a = 1;
    int b = 2;

    public TestClass(int a) {
        this.a = a + 1;
    }

    public void add1 () {
        a++;
        b++;
        throw new RuntimeException("test exception");
    }

    public void add2 () {
        a++;
        b++;
    }

    public native void jni_test();

}
