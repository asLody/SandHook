package com.swift.sandhook.xposedcompat.hookstub;

public interface CallOriginCallBack {
    Object call(long... args) throws Throwable;
}
