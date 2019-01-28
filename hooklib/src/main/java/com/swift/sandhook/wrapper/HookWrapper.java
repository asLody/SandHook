package com.swift.sandhook.wrapper;

import android.text.TextUtils;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.annotation.HookClass;
import com.swift.sandhook.annotation.HookMethod;
import com.swift.sandhook.annotation.HookMethodBackup;
import com.swift.sandhook.annotation.HookReflectClass;
import com.swift.sandhook.annotation.MethodParams;
import com.swift.sandhook.annotation.MethodReflectParams;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class HookWrapper {

    static Map<Member,HookEntity> globalHookEntityMap = new ConcurrentHashMap<>();

    public static void addHookClass(Class<?>... classes) throws HookErrorException {
        addHookClass(null, classes);
    }

    public static void addHookClass(ClassLoader classLoader, Class<?>... classes) throws HookErrorException {
        for (Class clazz:classes) {
            addHookClass(classLoader, clazz);
        }
    }

    public static void addHookClass(ClassLoader classLoader, Class<?> clazz) throws HookErrorException {
        Class targetHookClass = getTargetHookClass(classLoader, clazz);
        if (targetHookClass == null)
            throw new HookErrorException("error hook wrapper class :" + clazz.getName());
        Map<Member,HookEntity> hookEntityMap = getHookMethods(targetHookClass, clazz);
        fillBackupMethod(clazz, hookEntityMap);
        for (HookEntity entity:hookEntityMap.values()) {
            if (entity.target != null && entity.hook != null) {
                SandHook.hook(entity.target, entity.hook, entity.backup);
                globalHookEntityMap.put(entity.target, entity);
            }
        }
    }

    private static void fillBackupMethod(Class<?> clazz, Map<Member, HookEntity> hookEntityMap) {
        Field[] fields = clazz.getDeclaredFields();
        if (fields == null || fields.length == 0)
            return;
        if (hookEntityMap.isEmpty())
            return;
        for (Field field:fields) {
            if (!field.getType().equals(Method.class))
                continue;
            if (!Modifier.isStatic(field.getModifiers()))
                continue;
            HookMethodBackup hookMethodBackup = field.getAnnotation(HookMethodBackup.class);
            if (hookMethodBackup == null)
                continue;
            for (HookEntity hookEntity:hookEntityMap.values()) {
                if (TextUtils.equals(hookEntity.target.getName(), hookMethodBackup.value()) && hookEntity.backup != null && samePars(field, hookEntity.pars)) {
                    field.setAccessible(true);
                    try {
                        field.set(null, hookEntity.backup);
                    } catch (IllegalAccessException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    private static Map<Member, HookEntity> getHookMethods(Class targetHookClass, Class<?> hookWrapperClass) throws HookErrorException {
        Map<Member,HookEntity> hookEntityMap = new HashMap<>();
        Method[] methods = hookWrapperClass.getDeclaredMethods();
        if (methods == null && methods.length == 0)
            throw new HookErrorException("error hook wrapper class :" + targetHookClass.getName());
        for (Method method:methods) {
            HookMethod hookMethodAnno = method.getAnnotation(HookMethod.class);
            HookMethodBackup hookMethodBackupAnno = method.getAnnotation(HookMethodBackup.class);
            String methodName;
            Member foundMethod;
            Class[] pars;
            if (hookMethodAnno != null) {
                methodName = hookMethodAnno.value();
                pars = parseMethodPars(method);
                try {
                    if (methodName.equals("<init>")) {
                        foundMethod = targetHookClass.getConstructor(pars);
                    } else {
                        foundMethod = targetHookClass.getDeclaredMethod(methodName, pars);
                    }
                } catch (NoSuchMethodException e) {
                    throw new HookErrorException("can not find target method: " + methodName, e);
                }
                checkSignature(foundMethod, method, pars);
                HookEntity entity = hookEntityMap.get(foundMethod);
                if (entity == null) {
                    entity = new HookEntity(foundMethod);
                    hookEntityMap.put(foundMethod, entity);
                }
                entity.hook = method;
            } else if (hookMethodBackupAnno != null) {
                methodName = hookMethodBackupAnno.value();
                pars = parseMethodPars(method);
                try {
                    if (methodName.equals("<init>")) {
                        foundMethod = targetHookClass.getConstructor(pars);
                    } else {
                        foundMethod = targetHookClass.getDeclaredMethod(methodName, pars);
                    }
                } catch (NoSuchMethodException e) {
                    throw new HookErrorException("can not find target method: " + methodName, e);
                }
                checkSignature(foundMethod, method, pars);
                HookEntity entity = hookEntityMap.get(foundMethod);
                if (entity == null) {
                    entity = new HookEntity(foundMethod);
                    hookEntityMap.put(foundMethod, entity);
                }
                entity.pars = pars;
                entity.backup = method;
            } else {
                continue;
            }
        }
        return hookEntityMap;
    }

    private static Class[] parseMethodPars(Method method) throws HookErrorException {
        MethodParams methodParams = method.getAnnotation(MethodParams.class);
        MethodReflectParams methodReflectParams = method.getAnnotation(MethodReflectParams.class);
        if (methodParams != null) {
            return methodParams.value();
        } else if (methodReflectParams != null) {
            if (methodReflectParams.value().length == 0)
                return null;
            Class[] pars = new Class[methodReflectParams.value().length];
            for (int i = 0;i < methodReflectParams.value().length; i++) {
                try {
                    pars[i] = Class.forName(methodReflectParams.value()[i]);
                } catch (ClassNotFoundException e) {
                    throw new HookErrorException("hook method pars error: " + method.getName(), e);
                }
            }
            return pars;
        } else {
            return null;
        }
    }

    private static Class[] parseMethodPars(Field field) throws HookErrorException {
        MethodParams methodParams = field.getAnnotation(MethodParams.class);
        MethodReflectParams methodReflectParams = field.getAnnotation(MethodReflectParams.class);
        if (methodParams != null) {
            return methodParams.value();
        } else if (methodReflectParams != null) {
            if (methodReflectParams.value().length == 0)
                return null;
            Class[] pars = new Class[methodReflectParams.value().length];
            for (int i = 0;i < methodReflectParams.value().length; i++) {
                try {
                    pars[i] = Class.forName(methodReflectParams.value()[i]);
                } catch (ClassNotFoundException e) {
                    throw new HookErrorException("hook method pars error: " + field.getName(), e);
                }
            }
            return pars;
        } else {
            return null;
        }
    }


    private static boolean samePars(Field field, Class[] par) {
        try {
            Class[] parsOnField = parseMethodPars(field);
            if (par == null)
                par = new Class[0];
            if (parsOnField == null)
                parsOnField = new Class[0];
            if (par.length != parsOnField.length)
                return false;
            for (int i = 0;i < par.length;i++) {
                if (par[i] != parsOnField[i])
                    return false;
            }
            return true;
        } catch (HookErrorException e) {
            return false;
        }
    }




    private static Class getTargetHookClass(ClassLoader classLoader, Class<?> hookWrapperClass) {
        HookClass hookClass = hookWrapperClass.getAnnotation(HookClass.class);
        HookReflectClass hookReflectClass = hookWrapperClass.getAnnotation(HookReflectClass.class);
        if (hookClass != null) {
            return hookClass.value();
        } else if (hookReflectClass != null) {
            try {
                if (classLoader == null) {
                    return Class.forName(hookReflectClass.value());
                } else {
                    return Class.forName(hookReflectClass.value(), true, classLoader);
                }
            } catch (ClassNotFoundException e) {
                return null;
            }
        } else {
            return null;
        }
    }

    public static void checkSignature(Member origin, Method fake, Class[] originPars) throws HookErrorException {
        if (!Modifier.isStatic(fake.getModifiers()))
            throw new HookErrorException("hook method must static! - " + fake.getName());
        if (origin instanceof Constructor) {
            if (!fake.getReturnType().equals(Void.TYPE))
                throw new HookErrorException("error return type! - " + fake.getName());
        } else if (origin instanceof Method) {
            Class originRet = ((Method)origin).getReturnType();
            if (originRet != fake.getReturnType() && !originRet.isAssignableFrom(originRet))
                throw new HookErrorException("error return type! - " + fake.getName());
        }
        Class[] fakePars = fake.getParameterTypes();
        if (fakePars == null)
            fakePars = new Class[0];
        if (originPars == null)
            originPars = new Class[0];
        if (originPars.length == 0 && fakePars.length == 0)
            return;
        int parOffset = 0;
        if (!Modifier.isStatic(origin.getModifiers())) {
            parOffset = 1;
            if (fakePars.length == 0)
                throw new HookErrorException("first par must be this! " + fake.getName());
            if (fakePars[0] != origin.getDeclaringClass() && !fakePars[0].isAssignableFrom(origin.getDeclaringClass()))
                throw new HookErrorException("first par must be this! " + fake.getName());
            if (fakePars.length != originPars.length + 1)
                throw new HookErrorException("hook method pars must match the origin method! " + fake.getName());
        } else {
            if (fakePars.length != originPars.length)
                throw new HookErrorException("hook method pars must match the origin method! " + fake.getName());
        }
        for (int i = 0;i < originPars.length;i++) {
            if (fakePars[i + parOffset] != originPars[i] && !fakePars[i + parOffset].isAssignableFrom(originPars[i]))
                throw new HookErrorException("hook method pars must match the origin method! " + fake.getName());
        }
    }

    public static class HookEntity {

        public Member target;
        public Method hook;
        public Method backup;

        public Class[] pars;

        public HookEntity(Member target) {
            this.target = target;
        }


        public boolean isCtor() {
            return target instanceof Constructor;
        }
    }

}
