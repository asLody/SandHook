package de.robv.android.xposed;

public class GeneClass_Template {
    public static java.lang.reflect.Member method;
    public static de.robv.android.xposed.XposedBridge.AdditionalHookInfo tAdditionalInfoObj;

    public static boolean backup(Object obj, int i) {
        return false;
    }

    public static boolean hook(Object obj, int i) throws Throwable {
        Throwable th;
        if (!de.robv.android.xposed.XposedBridge.disableHooks) {
            Object[] snapshot = tAdditionalInfoObj.callbacks.getSnapshot();
            int length = snapshot.length;
            if (length != 0) {
                de.robv.android.xposed.XC_MethodHook.MethodHookParam methodHookParam = new de.robv.android.xposed.XC_MethodHook.MethodHookParam();
                methodHookParam.method = method;
                Object[] objArr = new Object[1];
                methodHookParam.args = objArr;
                methodHookParam.thisObject = obj;
                objArr[0] = Integer.valueOf(i);
                int i2 = 0;
                do {
                    try {
                        ((de.robv.android.xposed.XC_MethodHook) snapshot[i2]).callBeforeHookedMethod(methodHookParam);
                        if (methodHookParam.returnEarly) {
                            i2++;
                            break;
                        }
                    } catch (Throwable th2) {
                        de.robv.android.xposed.XposedBridge.log(th2);
                        methodHookParam.setResult(null);
                        methodHookParam.returnEarly = false;
                    }
                    i2++;
                } while (i2 < length);
                if (!methodHookParam.returnEarly) {
                    try {
                        methodHookParam.setResult(Boolean.valueOf(backup(obj, i)));
                    } catch (Throwable th3) {
                        methodHookParam.setThrowable(th3);
                    }
                }
                i2--;
                do {
                    Object result = methodHookParam.getResult();
                    Throwable th2 = methodHookParam.getThrowable();
                    try {
                        ((de.robv.android.xposed.XC_MethodHook) snapshot[i2]).callAfterHookedMethod(methodHookParam);
                    } catch (Throwable th4) {
                        de.robv.android.xposed.XposedBridge.log(th4);
                        if (th2 == null) {
                            methodHookParam.setResult(result);
                        } else {
                            methodHookParam.setThrowable(th2);
                        }
                    }
                    i2--;
                } while (i2 >= 0);
                if (!methodHookParam.hasThrowable()) {
                    return ((Boolean) methodHookParam.getResult()).booleanValue();
                }
                throw methodHookParam.getThrowable();
            }
        }
        return backup(obj, i);
    }

    public static void setup(java.lang.reflect.Member member, de.robv.android.xposed.XposedBridge.AdditionalHookInfo additionalHookInfo) {
        method = member;
        tAdditionalInfoObj = additionalHookInfo;
    }
}