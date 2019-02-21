package com.swift.sandhook.xposedcompat.hookstub;

import static com.swift.sandhook.xposedcompat.hookstub.HookStubManager.hookBridge;
import static com.swift.sandhook.xposedcompat.hookstub.HookStubManager.getMethodId;
import static com.swift.sandhook.xposedcompat.hookstub.HookStubManager.originMethods;
import static com.swift.sandhook.xposedcompat.utils.DexLog.printCallOriginError;

public class MethodHookerStubs64 {

    static class call_origin_5_0 implements CallOriginCallBack {
        @Override
        public long call(long... args) throws Throwable {
            return stub_backup_0(args[0], args[1], args[2], args[3], args[4]);
        }
    }
    public static long stub_hook_0(long a, long b, long c, long d, long e) throws Throwable {
        return  hookBridge(getMethodId(5, 0), new call_origin_5_0(), a, b, c, d, e);
    }
    public static long stub_backup_0(long a, long b, long c, long d, long e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 0)]);
        } catch (Throwable throwable) {}
        return 0;
    }


    static class call_origin_5_1 implements CallOriginCallBack {
        @Override
        public long call(long... args) throws Throwable {
            return stub_backup_1(args[0], args[1], args[2], args[3], args[4]);
        }
    }
    public static long stub_hook_1(long a, long b, long c, long d, long e) throws Throwable {
        return  hookBridge(getMethodId(5, 1), new call_origin_5_1(), a, b, c, d, e);
    }
    public static long stub_backup_1(long a, long b, long c, long d, long e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 1)]);
        } catch (Throwable throwable) {}
        return 0;
    }

    static class call_origin_5_2 implements CallOriginCallBack {
        @Override
        public long call(long... args) throws Throwable {
            return stub_backup_2(args[0], args[1], args[2], args[3], args[4]);
        }
    }
    public static long stub_hook_2(long a, long b, long c, long d, long e) throws Throwable {
        return  hookBridge(getMethodId(5, 2), new call_origin_5_2(), a, b, c, d, e);
    }
    public static long stub_backup_2(long a, long b, long c, long d, long e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 2)]);
        } catch (Throwable throwable) {}
        return 0;
    }



    static class call_origin_5_3 implements CallOriginCallBack {
        @Override
        public long call(long... args) throws Throwable {
            return stub_backup_3(args[0], args[1], args[2], args[3], args[4]);
        }
    }
    public static long stub_hook_3(long a, long b, long c, long d, long e) throws Throwable {
        return  hookBridge(getMethodId(5, 3), new call_origin_5_3(), a, b, c, d, e);
    }
    public static long stub_backup_3(long a, long b, long c, long d, long e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 3)]);
        } catch (Throwable throwable) {}
        return 0;
    }



    static class call_origin_5_4 implements CallOriginCallBack {
        @Override
        public long call(long... args) throws Throwable {
            return stub_backup_4(args[0], args[1], args[2], args[3], args[4]);
        }
    }
    public static long stub_hook_4(long a, long b, long c, long d, long e) throws Throwable {
        return  hookBridge(getMethodId(5, 4), new call_origin_5_4(), a, b, c, d, e);
    }
    public static long stub_backup_4(long a, long b, long c, long d, long e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 4)]);
        } catch (Throwable throwable) {}
        return 0;
    }


    static class call_origin_5_5 implements CallOriginCallBack {
        @Override
        public long call(long... args) throws Throwable {
            return stub_backup_5(args[0], args[1], args[2], args[3], args[4]);
        }
    }
    public static long stub_hook_5(long a, long b, long c, long d, long e) throws Throwable {
        return  hookBridge(getMethodId(5, 5), new call_origin_5_5(), a, b, c, d, e);
    }
    public static long stub_backup_5(long a, long b, long c, long d, long e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 5)]);
        } catch (Throwable throwable) {}
        return 0;
    }

    static class call_origin_5_6 implements CallOriginCallBack {
        @Override
        public long call(long... args) throws Throwable {
            return stub_backup_6(args[0], args[1], args[2], args[3], args[4]);
        }
    }
    public static long stub_hook_6(long a, long b, long c, long d, long e) throws Throwable {
        return  hookBridge(getMethodId(5, 6), new call_origin_5_5(), a, b, c, d, e);
    }
    public static long stub_backup_6(long a, long b, long c, long d, long e) throws Throwable {
        try {
            printCallOriginError(originMethods[getMethodId(5, 6)]);
        } catch (Throwable throwable) {}
        return 0;
    }
}
