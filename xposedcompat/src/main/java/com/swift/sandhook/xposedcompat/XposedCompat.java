package com.swift.sandhook.xposedcompat;

import android.app.Application;
import android.content.Context;

import com.swift.sandhook.xposedcompat.classloaders.ComposeClassLoader;
import com.swift.sandhook.xposedcompat.classloaders.XposedClassLoader;
import com.swift.sandhook.xposedcompat.utils.ApplicationUtils;
import com.swift.sandhook.xposedcompat.utils.ProcessUtils;

import java.io.File;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedInit;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class XposedCompat {

    public static File cacheDir;
    public static Context context;
    public static ClassLoader classLoader;
    public static boolean isFirstApplication;

    private static ClassLoader sandHookXposedClassLoader;


    public static void loadModule(String apkPath, ClassLoader classLoader) {
        XposedInit.loadModule(apkPath, classLoader);
    }


    public static void addXposedModuleCallback(IXposedHookLoadPackage module) {
        XposedBridge.hookLoadPackage(new IXposedHookLoadPackage.Wrapper(module));
    }

    public static void callXposedModuleInit() throws Throwable {
        //prepare LoadPackageParam
        XC_LoadPackage.LoadPackageParam packageParam = new XC_LoadPackage.LoadPackageParam(XposedBridge.sLoadedPackageCallbacks);
        Application application = ApplicationUtils.currentApplication();

        if (application != null) {
            if (packageParam.processName == null) {
                packageParam.processName = ProcessUtils.getProcessName(application);
            }
            if (packageParam.classLoader == null) {
                packageParam.classLoader = application.getClassLoader();
            }
            if (packageParam.appInfo == null) {
                packageParam.appInfo = application.getApplicationInfo();
            }

            if (cacheDir == null) {
                application.getCacheDir();
            }
        }
        XC_LoadPackage.callAll(packageParam);
    }

    public static ClassLoader getSandHookXposedClassLoader(ClassLoader appOriginClassLoader, ClassLoader sandBoxHostClassLoader) {
        if (sandHookXposedClassLoader != null) {
            return sandHookXposedClassLoader;
        } else {
            ClassLoader xposedClassLoader = getXposedClassLoader(sandBoxHostClassLoader);
            sandHookXposedClassLoader = new ComposeClassLoader(xposedClassLoader, appOriginClassLoader);
            return sandHookXposedClassLoader;
        }
    }

    private static XposedClassLoader xposedClassLoader;
    public static synchronized ClassLoader getXposedClassLoader(ClassLoader hostClassLoader) {
        if (xposedClassLoader == null) {
            xposedClassLoader = new XposedClassLoader(hostClassLoader);
        }
        return xposedClassLoader;
    }

}
