package com.swift.sandhook.xposedcompat.hookstub;

public interface CallOriginCallBack {
    int call(long... args) throws Throwable;
}
