package com.swift.sandhook.xposedcompat;

import android.app.Application;
import android.content.Context;

import com.swift.sandhook.xposedcompat.classloaders.ComposeClassLoader;
import com.swift.sandhook.xposedcompat.methodgen.DynamicBridge;
import com.swift.sandhook.xposedcompat.utils.ApplicationUtils;
import com.swift.sandhook.xposedcompat.utils.FileUtils;
import com.swift.sandhook.xposedcompat.utils.ProcessUtils;

import java.io.File;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedInit;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

import static com.swift.sandhook.xposedcompat.utils.DexMakerUtils.MD5;

public class XposedCompat {

    public static File cacheDir;
    public static Context context;
    public static volatile ClassLoader classLoader;
    public static String packageName;
    public static String processName;
    public static boolean isFirstApplication;

    //try to use internal stub hooker & backup method to speed up hook
    public static volatile boolean useInternalStub = true;
    public static volatile boolean useNewCallBackup = true;
    public static volatile boolean retryWhenCallOriginError = false;

    private static ClassLoader sandHookXposedClassLoader;


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
            sandHookXposedClassLoader = new ComposeClassLoader(sandBoxHostClassLoader, appOriginClassLoader);
            return sandHookXposedClassLoader;
        }
    }

    public static File getCacheDir() {
        if (cacheDir == null) {
            if (context == null) {
                context = ApplicationUtils.currentApplication();
            }
            if (context != null) {
                cacheDir = new File(context.getCacheDir(), MD5(processName != null ? processName : ProcessUtils.getProcessName(context)));
            }
        }
        return cacheDir;
    }

    public static boolean clearCache() {
        try {
            FileUtils.delete(getCacheDir());
            getCacheDir().mkdirs();
            return true;
        } catch (Throwable throwable) {
            return false;
        }
    }

    public static void clearOatCache() {
        DynamicBridge.clearOatFile();
    }

}
