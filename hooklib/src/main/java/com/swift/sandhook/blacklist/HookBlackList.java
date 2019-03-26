package com.swift.sandhook.blacklist;

import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class HookBlackList {

    public static Set<String> methodBlackList = new HashSet<>();
    public static Set<Class> classBlackList = new HashSet<>();

    static {
        methodBlackList.add("java.lang.reflect.Member.getDeclaringClass");
        methodBlackList.add("java.lang.reflect.Method.invoke");
        methodBlackList.add("java.lang.Class.getDeclaredField");
        methodBlackList.add("java.util.Map.get");
        methodBlackList.add("java.util.concurrent.ConcurrentHashMap.get");
        methodBlackList.add("java.lang.reflect.AccessibleObject.setAccessible");
        methodBlackList.add("java.lang.reflect.Member.getModifiers");
        methodBlackList.add("java.lang.reflect.InvocationTargetException.getCause");
        methodBlackList.add("java.lang.reflect.Method.hashCode");
        methodBlackList.add("java.lang.reflect.Class.getName");
        methodBlackList.add("java.lang.String.hashCode");
        methodBlackList.add("java.lang.String.length");
    }

    public final static boolean canNotHook(Member origin) {
        if (classBlackList.contains(origin.getDeclaringClass()))
            return true;
        String name = origin.getDeclaringClass().getName() + "." + origin.getName();
        return methodBlackList.contains(name);
    }

}
