package com.swift.sandhook.xposedcompat.hookstub;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.SandHookMethodResolver;
import com.swift.sandhook.utils.ParamWrapper;
import com.swift.sandhook.xposedcompat.utils.DexLog;

import java.lang.reflect.Constructor;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;

import static de.robv.android.xposed.XposedBridge.sHookedMethodCallbacks;

public class HookStubManager {


    public final static int MAX_STUB_ARGS = 8;

    public final static int[] stubSizes = new int[] {
            10,20,30,20,10,20,20,5,5
    };

    public final static AtomicInteger[] curUseStubIndexes = new AtomicInteger[MAX_STUB_ARGS + 1];

    public static int ALL_STUB = 0;

    public static Member[] originMethods;
    public static HookMethodEntity[] hookMethodEntities;

    private static final Map<Member, XposedBridge.CopyOnWriteSortedSet<XC_MethodHook>> hookCallbacks
            = sHookedMethodCallbacks;

    static {
        for (int i = 0;i < MAX_STUB_ARGS + 1;i++) {
            curUseStubIndexes[i] = new AtomicInteger(0);
            ALL_STUB += stubSizes[i];
        }
        originMethods = new Member[ALL_STUB];
        hookMethodEntities = new HookMethodEntity[ALL_STUB];
    }


    public static HookMethodEntity getHookMethodEntity(Member origin) {

        if (!support()) {
            return null;
        }

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
        } else {
            parType = new Class[0];
        }

        synchronized (HookStubManager.class) {
            StubMethodsInfo stubMethodInfo = getStubMethodPair(SandHook.is64Bit(), needStubArgCount);
            if (stubMethodInfo == null)
                return null;
            HookMethodEntity entity = new HookMethodEntity(origin, stubMethodInfo.hook, stubMethodInfo.backup);
            entity.retType = retType;
            entity.parType = parType;
            int id = getMethodId(stubMethodInfo.args, stubMethodInfo.index);
            originMethods[id] = origin;
            hookMethodEntities[id] = entity;
            if (tryCompileAndResolveCallOriginMethod(entity.backup, stubMethodInfo.args, stubMethodInfo.index)) {
                return entity;
            } else {
                DexLog.w("internal stub <" + entity.hook.getName() + "> call origin compile failure, skip use internal stub");
                return null;
            }
        }
    }

    public static int getMethodId(int args, int index) {
        int id = index;
        for (int i = 0;i < args;i++) {
            id += stubSizes[i];
        }
        return id;
    }

    public static String getHookMethodName(int index) {
        return "stub_hook_" + index;
    }

    public static String getBackupMethodName(int index) {
        return "stub_backup_" + index;
    }

    public static String getCallOriginClassName(int args, int index) {
        return "call_origin_" + args + "_" + index;
    }


    static class StubMethodsInfo {
        int args = 0;
        int index = 0;
        Method hook;
        Method backup;

        public StubMethodsInfo(int args, int index, Method hook, Method backup) {
            this.args = args;
            this.index = index;
            this.hook = hook;
            this.backup = backup;
        }
    }

    private static synchronized StubMethodsInfo getStubMethodPair(boolean is64Bit, int stubArgs) {

        stubArgs = getMatchStubArgsCount(stubArgs);

        if (stubArgs < 0)
            return null;

        int curUseStubIndex = curUseStubIndexes[stubArgs].getAndIncrement();
        Class[] pars = getFindMethodParTypes(is64Bit, stubArgs);
        try {
            if (is64Bit) {
                Method hook = MethodHookerStubs64.class.getDeclaredMethod(getHookMethodName(curUseStubIndex), pars);
                Method backup = MethodHookerStubs64.class.getDeclaredMethod(getBackupMethodName(curUseStubIndex), pars);
                if (hook == null || backup == null)
                    return null;
                return new StubMethodsInfo(stubArgs, curUseStubIndex, hook, backup);
            } else {
                Method hook = MethodHookerStubs32.class.getDeclaredMethod(getHookMethodName(curUseStubIndex), pars);
                Method backup = MethodHookerStubs32.class.getDeclaredMethod(getBackupMethodName(curUseStubIndex), pars);
                if (hook == null || backup == null)
                    return null;
                return new StubMethodsInfo(stubArgs, curUseStubIndex, hook, backup);
            }
        } catch (Exception e) {
            return null;
        }
    }

    public static Method getCallOriginMethod(int args, int index) {
        String className = SandHook.is64Bit() ? MethodHookerStubs64.class.getName() : MethodHookerStubs32.class.getName();
        className += "$";
        className += getCallOriginClassName(args, index);
        try {
            Class callOriginClass = Class.forName(className);
            return callOriginClass.getDeclaredMethod("call", long[].class);
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public static boolean tryCompileAndResolveCallOriginMethod(Method backupMethod, int args, int index) {
        Method method = getCallOriginMethod(args, index);
        if (method != null) {
            SandHookMethodResolver.resolveMethod(method, backupMethod);
            return SandHook.compileMethod(method);
        } else {
            return false;
        }
    }

    public static int getMatchStubArgsCount(int stubArgs) {
        for (int i = stubArgs;i <= MAX_STUB_ARGS;i++) {
            if (curUseStubIndexes[i].get() < stubSizes[i])
                return i;
        }
        return -1;
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

    public static long hookBridge(int id, CallOriginCallBack callOrigin, long... stubArgs) throws Throwable {

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
                param.setResult(entity.getResult(callOrigin.call(newArgs)));
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
        if (!param.hasThrowable()) {
            return entity.getResultAddress(param.getResult());
        } else {
            throw param.getThrowable();
        }
    }

    private static boolean hasArgs(long... args) {
        return args != null && args.length > 0;
    }

    public static boolean support() {
        return SandHook.canGetObject() && SandHook.canGetObjectAddress();
    }

}
