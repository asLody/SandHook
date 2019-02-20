package com.swift.sandhook.xposedcompat.hookstub;

import android.util.Pair;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.utils.ParamWrapper;

import java.lang.reflect.Constructor;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;

public class HookStubManager {


    public final static int MAX_STUB_ARGS = 4;
    public final static int MAX_PER_STUB = 5;

    public final static int MAX_STUB_COUNT = MAX_PER_STUB * (MAX_STUB_ARGS + 1);

    public final static AtomicInteger[] curUseStubIndexes = new AtomicInteger[MAX_STUB_ARGS + 1];

    public static Member[] originMethods = new Member[MAX_STUB_COUNT];
    public static HookMethodEntity[] hookMethodEntities = new HookMethodEntity[MAX_STUB_COUNT];

    private static final Map<Member, XposedBridge.CopyOnWriteSortedSet<XC_MethodHook>> hookCallbacks
            = new HashMap<>();

    static {
        for (int i = 0;i < curUseStubIndexes.length;i++) {
            curUseStubIndexes[i] = new AtomicInteger(0);
        }
    }


    public static HookMethodEntity getHookMethodEntity(Member origin) {

        Class[] parType;
        Class retType;
        boolean isStatic = Modifier.isStatic(origin.getModifiers());

        if (origin instanceof Method) {
            Method method = (Method) origin;
            retType = method.getReturnType();
            parType = method.getParameterTypes();
        } else if (origin instanceof Constructor) {
            Constructor constructor = (Constructor) origin;
            retType = Void.TYPE;
            parType = constructor.getParameterTypes();
        } else {
            return null;
        }

        if (!ParamWrapper.support(retType))
            return null;

        int needStubArgCount = isStatic ? 0 : 1;

        if (parType != null) {
            needStubArgCount += parType.length;
            if (needStubArgCount > MAX_STUB_ARGS)
                return null;
            for (Class par:parType) {
                if (!ParamWrapper.support(par))
                    return null;
            }
        }

        synchronized (HookStubManager.class) {
            Pair<Method,Method> methodPair = getStubMethodPair(SandHook.is64Bit(), needStubArgCount);
            if (methodPair == null)
                return null;
            HookMethodEntity entity = new HookMethodEntity(origin, methodPair.first, methodPair.second);
            entity.retType = retType;
            entity.parType = parType;
            return entity;
        }
    }

    public static int getMethodId(int args, int index) {
        return args * MAX_PER_STUB + index;
    }

    public static String getHookMethodName(int index) {
        return "stub_hook_" + index;
    }

    public static String getBackupMethodName(int index) {
        return "stub_backup_" + index;
    }

    private static synchronized Pair<Method,Method> getStubMethodPair(boolean is64Bit, int stubArgs) {
        int curUseStubIndex = curUseStubIndexes[stubArgs].getAndIncrement();
        if (curUseStubIndex >= MAX_PER_STUB)
            return null;
        Class[] pars = getFindMethodParTypes(is64Bit, stubArgs);
        try {
            if (is64Bit) {
                Method hook = MethodHookerStubs64.class.getDeclaredMethod(getHookMethodName(curUseStubIndex), pars);
                Method backup = MethodHookerStubs64.class.getDeclaredMethod(getBackupMethodName(curUseStubIndex), pars);
                if (hook == null || backup == null)
                    return null;
                return new Pair<>(hook, backup);
            } else {
                Method hook = MethodHookerStubs32.class.getDeclaredMethod(getHookMethodName(curUseStubIndex), pars);
                Method backup = MethodHookerStubs32.class.getDeclaredMethod(getBackupMethodName(curUseStubIndex), pars);
                if (hook == null || backup == null)
                    return null;
                return new Pair<>(hook, backup);
            }
        } catch (Exception e) {
            return null;
        }
    }

    public static Class[] getFindMethodParTypes(boolean is64Bit, int stubArgs) {
        if (stubArgs == 0)
            return null;
        Class[] args = new Class[stubArgs];
        if (is64Bit) {
            for (int i = 0;i < stubArgs;i++) {
                args[i] = long.class;
            }
        } else {
            for (int i = 0;i < stubArgs;i++) {
                args[i] = int.class;
            }
        }
        return args;
    }

    public static Object hookBridge(int id, CallOriginCallBack callOrigin, long... stubArgs) throws Throwable {

        if (XposedBridge.disableHooks)
            return callOrigin.call(stubArgs);

        Member originMethod = originMethods[id];
        HookMethodEntity entity = hookMethodEntities[id];

        Object[] snapshot = hookCallbacks.get(originMethod).getSnapshot();
        if (snapshot == null || snapshot.length == 0)
            return callOrigin.call(stubArgs);

        XC_MethodHook.MethodHookParam param = new XC_MethodHook.MethodHookParam();

        param.method  = originMethod;

        if (hasArgs(stubArgs)) {
            param.thisObject = entity.getThis(stubArgs[0]);
            param.args = entity.getArgs(stubArgs);
        }

        int beforeIdx = 0;
        do {
            try {
                ((XC_MethodHook) snapshot[beforeIdx]).callBeforeHookedMethod(param);
            } catch (Throwable t) {
                // reset result (ignoring what the unexpectedly exiting callback did)
                param.setResult(null);
                param.returnEarly = false;
                continue;
            }

            if (param.returnEarly) {
                // skip remaining "before" callbacks and corresponding "after" callbacks
                beforeIdx++;
                break;
            }
        } while (++beforeIdx < snapshot.length);

        // call original method if not requested otherwise
        if (!param.returnEarly) {
            try {
                //prepare new args
                long[] newArgs = entity.getArgsAddress(stubArgs, param.args);
                param.setResult(callOrigin.call(newArgs));
            } catch (Exception e) {
                XposedBridge.log(e);
                param.setThrowable(e);
            }
        }

        // call "after method" callbacks
        int afterIdx = beforeIdx - 1;
        do {
            Object lastResult =  param.getResult();
            Throwable lastThrowable = param.getThrowable();

            try {
                ((XC_MethodHook) snapshot[afterIdx]).callAfterHookedMethod(param);
            } catch (Throwable t) {
                XposedBridge.log(t);
                if (lastThrowable == null)
                    param.setResult(lastResult);
                else
                    param.setThrowable(lastThrowable);
            }
        } while (--afterIdx >= 0);

        if (param.hasThrowable()) {
            return param.getResult();
        } else {
            final Object result = param.getResult();
            return result;
        }
    }

    private static boolean hasArgs(long... args) {
        return args != null && args.length > 0;
    }

}
