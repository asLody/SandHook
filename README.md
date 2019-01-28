# SandHook
Android ART Hook

[ ![Download](https://api.bintray.com/packages/ganyao114/maven/hooklib/images/download.svg) ](https://bintray.com/ganyao114/maven/hooklib/_latestVersion)

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
implementation 'com.swift.sandhook:hooklib:0.0.1'
```

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

and
SandHook.addHookClass(CtrHook.class, LogHooker.class, CustmizeHooker.class, ActivityHooker.class, ObjectHooker.class);

you can also use:
SanHook.public static boolean hook(Member target, Method hook, Method backup) {}

```

if hookers is in plugin(like xposed):  

```groovy
provided 'com.swift.sandhook:hookannotation:0.0.1'
```
  
in your plugin

if OS <= 5.1 
backup method can call itself to avoid be inlining

# References

- Epic:https://github.com/tiann/epic
- Yahfa:https://github.com/rk700/YAHFA


