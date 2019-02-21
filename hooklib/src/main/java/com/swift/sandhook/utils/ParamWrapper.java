package com.swift.sandhook.utils;

import com.swift.sandhook.SandHook;

public class ParamWrapper {

    public static boolean support(Class objectType) {
        if (SandHook.is64Bit()) {
            return objectType != float.class && objectType != double.class;
        } else {
            return objectType != float.class && objectType != double.class && objectType != long.class;
        }
    }

    public static Object addressToObject(Class objectType, long address) {
        if (SandHook.is64Bit()) {
            return addressToObject64(objectType, address);
        } else {
            return addressToObject32(objectType, (int) address);
        }
    }

    public static Object addressToObject64(Class objectType, long address) {
        if (objectType == null)
            return null;
        if (objectType.isPrimitive()) {
            if (objectType == int.class) {
                return (int)address;
            } else if (objectType == long.class) {
                return address;
            } else if (objectType == short.class) {
                return (short)address;
            } else if (objectType == byte.class) {
                return (byte)address;
            } else if (objectType == char.class) {
                return (char)address;
            } else if (objectType == boolean.class) {
                return address != 0;
            } else {
                throw new RuntimeException("unknown type: " + objectType.toString());
            }
        } else {
            return SandHook.getObject(address);
        }
    }

    public static Object addressToObject32(Class objectType, int address) {
        if (objectType == null)
            return null;
        if (objectType.isPrimitive()) {
            if (objectType == int.class) {
                return address;
            } else if (objectType == short.class) {
                return (short)address;
            } else if (objectType == byte.class) {
                return (byte)address;
            } else if (objectType == char.class) {
                return (char)address;
            } else if (objectType == boolean.class) {
                return address != 0;
            } else {
                throw new RuntimeException("unknown type: " + objectType.toString());
            }
        } else {
            return SandHook.getObject(address);
        }
    }

    public static long objectToAddress(Class objectType, Object object) {
        if (SandHook.is64Bit()) {
            return objectToAddress64(objectType, object);
        } else {
            return objectToAddress32(objectType, object);
        }
    }

    public static int objectToAddress32(Class objectType, Object object) {
        if (object == null)
            return 0;
        if (objectType.isPrimitive()) {
            if (objectType == int.class) {
                return (int) object;
            } else if (objectType == short.class) {
                return (short)object;
            } else if (objectType == byte.class) {
                return (byte)object;
            } else if (objectType == char.class) {
                return (char)object;
            } else if (objectType == boolean.class) {
                return object == Boolean.TRUE ? 1 : 0;
            } else {
                throw new RuntimeException("unknown type: " + objectType.toString());
            }
        } else {
            return (int) SandHook.getObjectAddress(object);
        }
    }

    public static long objectToAddress64(Class objectType, Object object) {
        if (object == null)
            return 0;
        if (objectType.isPrimitive()) {
            if (objectType == int.class) {
                return (int)object;
            } else if (objectType == long.class) {
                return (long) object;
            } else if (objectType == short.class) {
                return (short)object;
            } else if (objectType == byte.class) {
                return (byte)object;
            } else if (objectType == char.class) {
                return (char)object;
            } else if (objectType == boolean.class) {
                return object == Boolean.TRUE ? 1 : 0;
            } else {
                throw new RuntimeException("unknown type: " + objectType.toString());
            }
        } else {
            return SandHook.getObjectAddress(object);
        }
    }

}
