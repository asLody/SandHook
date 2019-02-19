# SandHook
Android ART Hook

[ ![Version](https://api.bintray.com/packages/ganyao114/maven/hooklib/images/download.svg) ](https://bintray.com/ganyao114/maven/hooklib/_latestVersion)

[中文文档以及实现](https://blog.csdn.net/ganyao939543405/article/details/86661040)

# arch support 

- ARM64
- ARM32(no tested)
- Thumb-2

# OS

5.0 - 9.0

# Scope

- Object Methods
- Static Methods
- Constructors
- System Methods
- JNI Methods

hook abstract method is not recommended, you can invoke its impl method.

cant hook if lined

# how to use

```gradle
implementation 'com.swift.sandhook:hooklib:2.5.2'
```

- Annotation API

--------------------------------------------------------------------

- hook method must be a static method
- first par must be this if method is not static
- method description must "same"(can be isAssignableFrom) with origin method
- backup method same with above

```java
@HookClass(Activity.class)
//@HookReflectClass("android.app.Activity")
public class ActivityHooker {
    
    // can invoke to call origin
    @HookMethodBackup("onCreate")
    @MethodParams(Bundle.class)
    static Method onCreateBackup;

    @HookMethodBackup("onPause")
    static Method onPauseBackup;

    @HookMethod("onCreate")
    @MethodParams(Bundle.class)
    //@MethodReflectParams("android.os.Bundle")
    public static void onCreate(Activity thiz, Bundle bundle) {
        Log.e("ActivityHooker", "hooked onCreate success " + thiz);
        onCreateBackup(thiz, bundle);
    }

    @HookMethodBackup("onCreate")
    @MethodParams(Bundle.class)
    public static void onCreateBackup(Activity thiz, Bundle bundle) {
        //invoke self to kill inline
        onCreateBackup(thiz, bundle);
    }

    @HookMethod("onPause")
    public static void onPause(Activity thiz) {
        Log.e("ActivityHooker", "hooked onPause success " + thiz);
        onPauseBackup(thiz);
    }

    @HookMethodBackup("onPause")
    public static void onPauseBackup(Activity thiz) {
        //invoke self to kill inline
        onPauseBackup(thiz);
    }

}


//or like this:

@HookClass(TestClass.class)
public class NewAnnotationApiHooker {

    @HookMethod("testNewHookApi")
    public static void onTestNewHookApi(@ThisObject TestClass thiz, @Param("com.swift.sandhook.MainActivity") Activity activity, int a) {
        Log.e("TestClassHook", "testNewHookApi been hooked");
        onTestNewHookApiBackup(thiz, activity, a);
    }

    @HookMethodBackup("testNewHookApi")
    public static void onTestNewHookApiBackup(@ThisObject TestClass thiz, @Param("com.swift.sandhook.MainActivity") Activity activity, int a) {
        onTestNewHookApiBackup(thiz, activity, a);
    }

}

and
SandHook.addHookClass(CtrHook.class, LogHooker.class, CustmizeHooker.class, ActivityHooker.class, ObjectHooker.class);

you can also use:
SanHook.public static boolean hook(Member target, Method hook, Method backup) {}

```

if hookers is in plugin(like xposed):  

```groovy
provided 'com.swift.sandhook:hookannotation:2.5.2'
```
  
in your plugin

if OS <= 5.1 
backup method can call itself to avoid be inlining

- Xposed API

--------------------------------------------------------------------

Now you can use Xposed api:  

```groovy
implementation 'com.swift.sandhook:xposedcompat:2.5.2'
```

```java
//setup for xposed
XposedCompat.cacheDir = getCacheDir();
XposedCompat.context = this;
XposedCompat.classLoader = getClassLoader();
XposedCompat.isFirstApplication= true;  
//do hook
XposedHelpers.findAndHookMethod(Activity.class, "onResume", new XC_MethodHook() {
      @Override
      protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
          super.beforeHookedMethod(param);
          Log.e("XposedCompat", "beforeHookedMethod: " + param.method.getName());
      }

      @Override
      protected void afterHookedMethod(MethodHookParam param) throws Throwable {
          super.afterHookedMethod(param);
          Log.e("XposedCompat", "afterHookedMethod: " + param.method.getName());
      }
});

```

# Notice
!!!!!!!!
when OS >= 8.0
you must call backup method in hook method, if you want call it in other method, please call  SandHook.compileMethod(otherMethod) before call backup method.
    
because when ART trigger JIT form profiling, JIT will invoke -> ResolveCompilingMethodsClass -> ClassLinker::ResolveMethod -> CheckIncompatibleClassChange -> ThrowIncompatibleClassChangeError fianlly!!!



# References

- Epic:https://github.com/tiann/epic
- Yahfa:https://github.com/rk700/YAHFA


