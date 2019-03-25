package com.swift.sandhook.blacklist;

import java.lang.reflect.Member;
import java.util.HashSet;
import java.util.Set;

public class HookBlackList {

    static Set<String> blackList = new HashSet<>();

    static {
        blackList.add("java.lang.reflect.Member.getDeclaringClass");
        blackList.add("java.lang.reflect.Method.invoke");
        blackList.add("java.util.Map.get");
        blackList.add("java.lang.reflect.AccessibleObject.setAccessible");
        blackList.add("java.lang.reflect.Member.getModifiers");
        blackList.add("java.lang.reflect.InvocationTargetException.getCause");
    }

    public final static boolean canNotHook(Member origin) {
        String name = origin.getDeclaringClass().getName() + "." + origin.getName();
        return blackList.contains(name);
    }

}
