package de.robv.android.xposed;

import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import dalvik.system.DexClassLoader;
import dalvik.system.DexFile;

import static de.robv.android.xposed.XposedHelpers.closeSilently;

public final class XposedInit {
    private static final String TAG = XposedBridge.TAG;
    private static final String INSTANT_RUN_CLASS = "com.android.tools.fd.runtime.BootstrapApplication";
    // TODO not supported yet
    private static boolean disableResources = true;

    private XposedInit() {
    }

    private static volatile AtomicBoolean bootstrapHooked = new AtomicBoolean(false);

    /**
     * Hook some methods which we want to create an easier interface for developers.
     */

    /*package*/
    static void hookResources() throws Throwable {
        // ed: not for now
    }

    private static boolean needsToCloseFilesForFork() {
        // ed: we always start to do our work after forking finishes
        return false;
    }

    /**
     * Try to load all modules defined in <code>INSTALLER_DATA_BASE_DIR/conf/modules.list</code>
     */
    private static volatile AtomicBoolean modulesLoaded = new AtomicBoolean(false);


    /**
     * Load a module from an APK by calling the init(String) method for all classes defined
     * in <code>assets/xposed_init</code>.
     */
    public static void loadModule(String modulePath, String moduleOdexDir, String moduleSoPath,ClassLoader topClassLoader) {

        if (!new File(modulePath).exists()) {
            Log.e(TAG, "  File does not exist");
            return;
        }

        DexFile dexFile;
        try {
            dexFile = new DexFile(modulePath);
        } catch (IOException e) {
            Log.e(TAG, "  Cannot load module", e);
            return;
        }

        if (dexFile.loadClass(INSTANT_RUN_CLASS, topClassLoader) != null) {
            Log.e(TAG, "  Cannot load module, please disable \"Instant Run\" in Android Studio.");
            closeSilently(dexFile);
            return;
        }

        if (dexFile.loadClass(XposedBridge.class.getName(), topClassLoader) != null) {
            Log.e(TAG, "  Cannot load module:");
            Log.e(TAG, "  The Xposed API classes are compiled into the module's APK.");
            Log.e(TAG, "  This may cause strange issues and must be fixed by the module developer.");
            Log.e(TAG, "  For details, see: http://api.xposed.info/using.html");
            closeSilently(dexFile);
            return;
        }

        closeSilently(dexFile);

        ZipFile zipFile = null;
        InputStream is;
        try {
            zipFile = new ZipFile(modulePath);
            ZipEntry zipEntry = zipFile.getEntry("assets/xposed_init");
            if (zipEntry == null) {
                Log.e(TAG, "  assets/xposed_init not found in the APK");
                closeSilently(zipFile);
                return;
            }
            is = zipFile.getInputStream(zipEntry);
        } catch (IOException e) {
            Log.e(TAG, "  Cannot read assets/xposed_init in the APK", e);
            closeSilently(zipFile);
            return;
        }

        ClassLoader mcl = new DexClassLoader(modulePath, moduleOdexDir, moduleSoPath, topClassLoader);
        BufferedReader moduleClassesReader = new BufferedReader(new InputStreamReader(is));
        try {
            String moduleClassName;
            while ((moduleClassName = moduleClassesReader.readLine()) != null) {
                moduleClassName = moduleClassName.trim();
                if (moduleClassName.isEmpty() || moduleClassName.startsWith("#"))
                    continue;

                try {
                    Log.i(TAG, "  Loading class " + moduleClassName);
                    Class<?> moduleClass = mcl.loadClass(moduleClassName);

                    if (!IXposedMod.class.isAssignableFrom(moduleClass)) {
                        Log.e(TAG, "    This class doesn't implement any sub-interface of IXposedMod, skipping it");
                        continue;
                    } else if (disableResources && IXposedHookInitPackageResources.class.isAssignableFrom(moduleClass)) {
                        Log.e(TAG, "    This class requires resource-related hooks (which are disabled), skipping it.");
                        continue;
                    }

                    final Object moduleInstance = moduleClass.newInstance();

                    if (true) {

                        //fake
                        if (moduleInstance instanceof IXposedHookZygoteInit) {
                            IXposedHookZygoteInit.StartupParam param = new IXposedHookZygoteInit.StartupParam();
                            param.modulePath = modulePath;
                            param.startsSystemServer = false;
                            ((IXposedHookZygoteInit) moduleInstance).initZygote(param);
                        }

                        if (moduleInstance instanceof IXposedHookLoadPackage)
                            XposedBridge.hookLoadPackage(new IXposedHookLoadPackage.Wrapper((IXposedHookLoadPackage) moduleInstance));

                        //not support now
                        //so off
                        if (moduleInstance instanceof IXposedHookInitPackageResources) {
                            throw new UnsupportedOperationException("can not hook resource!");
                        }
                    } else {
                        //not support now
                        //so off
                        if (moduleInstance instanceof IXposedHookCmdInit) {
                            throw new UnsupportedOperationException("can not hook cmd!");
                        }
                    }
                } catch (Throwable t) {
                    Log.e(TAG, "    Failed to load class " + moduleClassName, t);
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "  Failed to load module from " + modulePath, e);
        } finally {
            closeSilently(is);
            closeSilently(zipFile);
        }
    }
}
