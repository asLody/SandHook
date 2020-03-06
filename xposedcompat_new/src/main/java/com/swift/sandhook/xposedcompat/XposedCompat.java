package com.swift.sandhook.xposedcompat;

import android.app.Application;
import android.content.Context;

import com.swift.sandhook.HookLog;
import com.swift.sandhook.SandHook;
import com.swift.sandhook.SandHookConfig;
import com.swift.sandhook.wrapper.HookErrorException;
import com.swift.sandhook.wrapper.HookWrapper;
import com.swift.sandhook.wrapper.StubMethodsFactory;
import com.swift.sandhook.xposedcompat.utils.ApplicationUtils;
import com.swift.sandhook.xposedcompat.utils.ClassUtils;
import com.swift.sandhook.xposedcompat.utils.ProxyClassLoader;
import com.swift.sandhook.xposedcompat.utils.ProcessUtils;

import java.lang.reflect.Constructor;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Arrays;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedInit;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

/**
 * @author Swift Gan
 * Create: 2019/6/3
 * Desc:
 */
public class XposedCompat {

    public volatile static boolean inited = false;
    public volatile static SandHookConfig.LibLoader libLoader = new SandHookConfig.LibLoader() {
        @Override
        public void loadLib() {
            System.loadLibrary("sandhook-xp");
        }
    };

    public static Context context;
    public static volatile ClassLoader classLoader;
    public static String packageName;
    public static String processName;
    public static boolean isFirstApplication;

    private static ClassLoader sandHookXposedClassLoader;

    private static volatile int curSlot = 0;
    public static volatile HookInfo[] hookInfos = new HookInfo[100];

    static {
        try {
            init();
        } catch (Throwable throwable) {
            throwable.printStackTrace();
        }
    }

    public static boolean init() throws Throwable {
        if (inited) return true;
        libLoader.loadLib();
        Method bridgeMethod = XposedCompat.class.getDeclaredMethod("hookBridge", int.class, Object.class, Object[].class);
        init(XposedCompat.class, bridgeMethod, Object.class);
        inited = true;
        return true;
    }

    private native static void init(Class bridgeClass, Method bridgeMethod, Class ObjClass);

    private native static long getJNITrampoline(int slot, boolean isStatic, char retShorty, char[] paramsShorty);

    private native static void addHookMethod(Member hookMethod);

    public static long getJNITrampoline(Member origin, int slot) {
        if (origin instanceof Constructor) {
            return getJNITrampoline(slot, false, ClassUtils.getShorty(Void.TYPE), ClassUtils.getShorties(((Constructor) origin).getParameterTypes()));
        } else {
            Method method = (Method) origin;
            return getJNITrampoline(slot, Modifier.isStatic(method.getModifiers()), ClassUtils.getShorty(method.getReturnType()), ClassUtils.getShorties(method.getParameterTypes()));
        }
    }

    private synchronized static int genSlot() {
        curSlot++;
        if (curSlot >= hookInfos.length) {
            hookInfos = Arrays.copyOf(hookInfos, hookInfos.length + 30);
        }
        return curSlot;
    }

    public synchronized static boolean hookMethod(Member origin, XposedBridge.AdditionalHookInfo additionalHookInfo) {
        Method hook = StubMethodsFactory.getStubMethod();
        Method backup = StubMethodsFactory.getStubMethod();
        int slot = genSlot();
        long jniTrampoline = getJNITrampoline(origin, slot);
        if (jniTrampoline == 0) {
            return false;
        }
        if (!SandHook.setNativeEntry(origin, hook, jniTrampoline)) {
            return false;
        }
        HookInfo hookInfo = new HookInfo();
        hookInfo.origin = origin;
        hookInfo.hook = hook;
        hookInfo.backup = backup;
        hookInfo.additionalHookInfo = additionalHookInfo;
        hookInfos[slot] = hookInfo;
        try {
            SandHook.hook(new HookWrapper.HookEntity(origin, hook, backup));
            addHookMethod(hook);
            return true;
        } catch (HookErrorException e) {
            HookLog.e("hook error!", e);
            return false;
        }
    }

    public static Object hookBridge(int slot, Object thiz, Object[] params) throws Throwable {
        HookInfo hookInfo = hookInfos[slot];

        if (XposedBridge.disableHooks) {
            return SandHook.callOriginMethod(hookInfo.origin, hookInfo.backup, thiz, params);
        }

        Object[] snapshot = hookInfo.additionalHookInfo.callbacks.getSnapshot();

        if (snapshot == null || snapshot.length == 0) {
            return SandHook.callOriginMethod(hookInfo.origin, hookInfo.backup, thiz, params);
        }

        XC_MethodHook.MethodHookParam param = new XC_MethodHook.MethodHookParam();

        param.method = hookInfo.origin;
        param.thisObject = thiz;
        param.args = params;

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
                param.setResult(SandHook.callOriginMethod(hookInfo.origin, hookInfo.backup, thiz, param.args));
            } catch (Throwable e) {
                XposedBridge.log(e);
                param.setThrowable(e);
            }
        }

        // call "after method" callbacks
        int afterIdx = beforeIdx - 1;
        do {
            Object lastResult = param.getResult();
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
            return param.getResult();
        } else {
            throw param.getThrowable();
        }
    }


    public static void loadModule(String modulePath, String moduleOdexDir, String moduleSoPath,ClassLoader classLoader) {
        XposedInit.loadModule(modulePath, moduleOdexDir, moduleSoPath, classLoader);
    }


    public static void addXposedModuleCallback(IXposedHookLoadPackage module) {
        XposedBridge.hookLoadPackage(new IXposedHookLoadPackage.Wrapper(module));
    }

    public static void callXposedModuleInit() throws Throwable {
        //prepare LoadPackageParam
        XC_LoadPackage.LoadPackageParam packageParam = new XC_LoadPackage.LoadPackageParam(XposedBridge.sLoadedPackageCallbacks);
        Application application = ApplicationUtils.currentApplication();


        if (application != null) {
            if (packageParam.packageName == null) {
                packageParam.packageName = application.getPackageName();
            }

            if (packageParam.processName == null) {
                packageParam.processName = ProcessUtils.getProcessName(application);
            }
            if (packageParam.classLoader == null) {
                packageParam.classLoader = application.getClassLoader();
            }
            if (packageParam.appInfo == null) {
                packageParam.appInfo = application.getApplicationInfo();
            }
        }
        XC_LoadPackage.callAll(packageParam);
    }

    public static ClassLoader getSandHookXposedClassLoader(ClassLoader appOriginClassLoader, ClassLoader sandBoxHostClassLoader) {
        if (sandHookXposedClassLoader != null) {
            return sandHookXposedClassLoader;
        } else {
            sandHookXposedClassLoader = new ProxyClassLoader(sandBoxHostClassLoader, appOriginClassLoader);
            return sandHookXposedClassLoader;
        }
    }

}
