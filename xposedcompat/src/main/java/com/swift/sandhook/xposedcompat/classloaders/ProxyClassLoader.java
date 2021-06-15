package com.swift.sandhook.xposedcompat.classloaders;

public class ProxyClassLoader extends ClassLoader {

    private ClassLoader mClassLoader;

    public ProxyClassLoader(ClassLoader parentCL, ClassLoader appCL) {
        super(parentCL);
        mClassLoader = appCL;
    }

    public ProxyClassLoader(ClassLoader parent)
    {
        super(parent);
    }

    public void setChild(ClassLoader child)
    {
        mClassLoader = child;
    }

    @Override
    protected Class<?> loadClass(String name, boolean resolve) throws ClassNotFoundException {
        Class clazz = null;

        try {
            clazz = mClassLoader.loadClass(name);
        } catch (ClassNotFoundException ignored) {
        }

        if (clazz == null) {
            clazz = super.loadClass(name, resolve);
            if (clazz == null) {
                throw new ClassNotFoundException("class not found in this scope "+name);
            }
        }

        return clazz;
    }
}
