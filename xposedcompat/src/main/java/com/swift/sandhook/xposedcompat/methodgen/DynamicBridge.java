package com.swift.sandhook.xposedcompat.methodgen;

import android.os.Trace;

import com.swift.sandhook.SandHook;
import com.swift.sandhook.blacklist.HookBlackList;
import com.swift.sandhook.wrapper.HookWrapper;
import com.swift.sandhook.xposedcompat.XposedCompat;
import com.swift.sandhook.xposedcompat.classloaders.ProxyClassLoader;
import com.swift.sandhook.xposedcompat.hookstub.HookMethodEntity;
import com.swift.sandhook.xposedcompat.hookstub.HookStubManager;
import com.swift.sandhook.xposedcompat.utils.DexLog;
import com.swift.sandhook.xposedcompat.utils.FileUtils;

import java.io.File;
import java.lang.reflect.Constructor;
import java.lang.reflect.Member;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

import de.robv.android.xposed.XposedBridge;

public final class DynamicBridge {

    private static HookMaker defaultHookMaker = XposedCompat.useNewCallBackup ? new HookerDexMakerNew() : new HookerDexMaker();
    private static final AtomicBoolean dexPathInited = new AtomicBoolean(false);
    private static File dexDir;

    //use internal stubs
    private final static Map<Member,HookMethodEntity> entityMap = new HashMap<>();
    //use dex maker
    private final static HashMap<Member, Method> hookedInfo = new HashMap<>();

    public static synchronized void hookMethod(Member hookMethod, XposedBridge.AdditionalHookInfo additionalHookInfo) {

        if (!checkMember(hookMethod)) {
            return;
        }

        if (hookedInfo.containsKey(hookMethod) || entityMap.containsKey(hookMethod)) {
            DexLog.w("already hook method:" + hookMethod.toString());
            return;
        }

        try {
            if (dexPathInited.compareAndSet(false, true)) {
                try {
                    String fixedAppDataDir = XposedCompat.getCacheDir().getAbsolutePath();
                    dexDir = new File(fixedAppDataDir, "/sandxposed/");
                    if (!dexDir.exists())
                        dexDir.mkdirs();
                } catch (Throwable throwable) {
                    DexLog.e("error when init dex path", throwable);
                }
            }
            Trace.beginSection("SandHook-Xposed");
            long timeStart = System.currentTimeMillis();
            HookMethodEntity stub = null;
            if (XposedCompat.useInternalStub && !HookBlackList.canNotHookByStub(hookMethod) && !HookBlackList.canNotHookByBridge(hookMethod)) {
                stub = HookStubManager.getHookMethodEntity(hookMethod, additionalHookInfo);
            }
            if (stub != null) {
                SandHook.hook(new HookWrapper.HookEntity(hookMethod, stub.hook, stub.backup, false));
                entityMap.put(hookMethod, stub);
            } else {
                HookMaker hookMaker;
                if (HookBlackList.canNotHookByBridge(hookMethod)) {
                    hookMaker = new HookerDexMaker();
                } else {
                    hookMaker = defaultHookMaker;
                }
                
                ProxyClassLoader loader = new ProxyClassLoader(hookMethod.getDeclaringClass().getClassLoader());
                loader.setChild(DynamicBridge.class.getClassLoader());

                hookMaker.start(hookMethod, additionalHookInfo,
                        // -----------
                        // 如果优先搜索当前类的类加载器，就会有相应方法类加载器复用
                        // 并且有的手机魔改了libcore，例如小米MIUI12的部分机型
                        // 造成提示XposedBridge.AdditionalHookInfo类加载器不对的问题。
                        // 用最简单的方法解决问题，你当然可以直接动态代理AdditionalHookInfo里面的callback
                        // 也可以合并Elements，你喜欢就行。
                        // 但是我们解决问题，优先采用简单粗暴的方法。
                        // QQ 647564826
                        // hookMethod classloader must be parent
                        loader,
                        dexDir == null ? null : dexDir.getAbsolutePath());
                hookedInfo.put(hookMethod, hookMaker.getCallBackupMethod());
            }
            DexLog.d("hook method <" + hookMethod.toString() + "> cost " + (System.currentTimeMillis() - timeStart) + " ms, by " + (stub != null ? "internal stub" : "dex maker"));
            Trace.endSection();
        } catch (Throwable e) {
            DexLog.e("error occur when hook method <" + hookMethod.toString() + ">", e);
        }
    }

    public static void clearOatFile() {
        String fixedAppDataDir = XposedCompat.getCacheDir().getAbsolutePath();
        File dexOatDir = new File(fixedAppDataDir, "/sandxposed/oat/");
        if (!dexOatDir.exists())
            return;
        try {
            FileUtils.delete(dexOatDir);
            dexOatDir.mkdirs();
        } catch (Throwable throwable) {
        }
    }

    private static boolean checkMember(Member member) {

        if (member instanceof Method) {
            return true;
        } else if (member instanceof Constructor<?>) {
            return true;
        } else if (member.getDeclaringClass().isInterface()) {
            DexLog.e("Cannot hook interfaces: " + member.toString());
            return false;
        } else if (Modifier.isAbstract(member.getModifiers())) {
            DexLog.e("Cannot hook abstract methods: " + member.toString());
            return false;
        } else {
            DexLog.e("Only methods and constructors can be hooked: " + member.toString());
            return false;
        }
    }
}


