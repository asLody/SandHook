package com.swift.sandhook.xposedcompat.classloaders;

/**
 * XposedClassLoader: make sure load xposed class in sandhook
 */
public class XposedClassLoader extends ClassLoader {

    private ClassLoader mHostClassLoader;
    public XposedClassLoader(ClassLoader hostClassLoader) {
        super(ClassLoader.getSystemClassLoader()); // parent is BootClassLoader
        mHostClassLoader = hostClassLoader;
    }

    @Override
    protected Class<?> loadClass(String name, boolean resolve) throws ClassNotFoundException {
        if (name.startsWith("de.robv.android.xposed") || name.startsWith("android")) {
            return mHostClassLoader.loadClass(name);
        }
        return super.loadClass(name, resolve);
    }
}
