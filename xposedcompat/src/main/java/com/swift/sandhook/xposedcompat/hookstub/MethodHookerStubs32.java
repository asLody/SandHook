package com.swift.sandhook.xposedcompat.hookstub;

import static com.swift.sandhook.xposedcompat.hookstub.HookStubManager.hookBridge;
import static com.swift.sandhook.xposedcompat.hookstub.HookStubManager.getMethodId;
import static com.swift.sandhook.xposedcompat.hookstub.HookStubManager.originMethods;
import static com.swift.sandhook.xposedcompat.utils.DexLog.printCallOriginError;

public class MethodHookerStubs32 {


    static class call_origin_5_0 implements CallOriginCallBack {
        @Override
        public int call(long... args) throws Throwable {
            return stub_backup_0((int)args[0], (int)args[1],(int) args[2], (int)args[3], (int)args[4]);
        }
    }
    public static int stub_hook_0(int a, int b, int c, int d, int e) throws Throwable {
        return (int) hookBridge(getMethodId(5, 0), new call_origin_5_0(), a, b, c, d, e);
    }
    public static int stub_backup_0(int a, int b, int c, int d, int e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 0)]);
        } catch (Throwable throwable) {}
        return 0;
    }


    static class call_origin_5_1 implements CallOriginCallBack {
        @Override
        public int call(long... args) throws Throwable {
            return stub_backup_1((int)args[0], (int)args[1],(int) args[2], (int)args[3], (int)args[4]);
        }
    }
    public static int stub_hook_1(int a, int b, int c, int d, int e) throws Throwable {
        return (int) hookBridge(getMethodId(5, 1), new call_origin_5_1(), a, b, c, d, e);
    }
    public static int stub_backup_1(int a, int b, int c, int d, int e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 1)]);
        } catch (Throwable throwable) {}
        return 0;
    }

    static class call_origin_5_2 implements CallOriginCallBack {
        @Override
        public int call(long... args) throws Throwable {
            return stub_backup_2((int)args[0], (int)args[1],(int) args[2], (int)args[3], (int)args[4]);
        }
    }
    public static int stub_hook_2(int a, int b, int c, int d, int e) throws Throwable {
        return (int) hookBridge(getMethodId(5, 2), new call_origin_5_2(), a, b, c, d, e);
    }
    public static int stub_backup_2(int a, int b, int c, int d, int e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 2)]);
        } catch (Throwable throwable) {}
        return 0;
    }



    static class call_origin_5_3 implements CallOriginCallBack {
        @Override
        public int call(long... args) throws Throwable {
            return stub_backup_3((int)args[0], (int)args[1],(int) args[2], (int)args[3], (int)args[4]);
        }
    }
    public static int stub_hook_3(int a, int b, int c, int d, int e) throws Throwable {
        return (int) hookBridge(getMethodId(5, 3), new call_origin_5_3(), a, b, c, d, e);
    }
    public static int stub_backup_3(int a, int b, int c, int d, int e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 3)]);
        } catch (Throwable throwable) {}
        return 0;
    }



    static class call_origin_5_4 implements CallOriginCallBack {
        @Override
        public int call(long... args) throws Throwable {
            return stub_backup_4((int)args[0], (int)args[1],(int) args[2], (int)args[3], (int)args[4]);
        }
    }
    public static int stub_hook_4(int a, int b, int c, int d, int e) throws Throwable {
        return (int) hookBridge(getMethodId(5, 4), new call_origin_5_4(), a, b, c, d, e);
    }
    public static int stub_backup_4(int a, int b, int c, int d, int e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 4)]);
        } catch (Throwable throwable) {}
        return 0;
    }


    static class call_origin_5_5 implements CallOriginCallBack {
        @Override
        public int call(long... args) throws Throwable {
            return stub_backup_5((int)args[0], (int)args[1],(int) args[2], (int)args[3], (int)args[4]);
        }
    }
    public static int stub_hook_5(int a, int b, int c, int d, int e) throws Throwable {
        return (int) hookBridge(getMethodId(5, 5), new call_origin_5_5(), a, b, c, d, e);
    }
    public static int stub_backup_5(int a, int b, int c, int d, int e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 5)]);
        } catch (Throwable throwable) {}
        return 0;
    }

}
