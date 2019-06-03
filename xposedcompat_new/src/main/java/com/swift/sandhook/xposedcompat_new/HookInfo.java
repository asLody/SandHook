package com.swift.sandhook.xposedcompat_new;

import java.lang.reflect.Member;
import java.lang.reflect.Method;

import de.robv.android.xposed.XposedBridge;

/**
 * @author Swift Gan
 * Create: 2019/6/3
 * Desc:
 */
public class HookInfo {
    public Member origin;
    public Method hook;
    public Method backup;
    public XposedBridge.AdditionalHookInfo additionalHookInfo;
}
