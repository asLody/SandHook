---
author: Swift Gan
title: Android ART Hook
date: 2019/04/01
---
# SandHook

Android Art Hook

Swift Gan

----

## Agenda

- 简介
- ART Invoke 指令生成
- 基本实现
- Xposed 支持
- 指令检查
- inline 处理
- Android Q
- Demo

----

## 简介

SandHook 是作用在 Android ART 虚拟机上的 Java 层 Hook 框架

https://github.com/ganyao114/SandHook

---

### OS
- 4.4(JNI 不支持 call 原方法)
- 5.0 - 5.1
- 6.0
- 7.0 - 7.1
- 8.0 - 8.1
- 9.0
- 10.0

---

### ARCH

- ARM32(基本见不到)
- THUMb32
- AARCH64

---

### 方法范围

- Object Methods
- Static Methods
- Constructors
- System Methods
- JNI Methods
- 不支持 abstract 方法

---

### 如何使用

```gradle
implementation 'com.swift.sandhook:hooklib:3.3.8'
implementation 'com.swift.sandhook:xposedcompat:3.3.8'
```

- Annotation API
- Xposed API

---

#### Annotation API


```java
@HookClass(Activity.class)
//@HookReflectClass("android.app.Activity")
public class ActivityHooker {

    @HookMethodBackup("onCreate")
    @MethodParams(Bundle.class)
    static Method onCreateBackup;

    @HookMethodBackup("onPause")
    static HookWrapper.HookEntity onPauseBackup;

    @HookMethod("onCreate")
    public static void onCreate(@ThisObject Activity thiz,
                                @Param("android.os.Bundle") Object bundle) throws Throwable {
        Log.e("ActivityHooker", "hooked onCreate success " + thiz);
        SandHook.callOriginByBackup(onCreateBackup, thiz, bundle);
    }

    @HookMethod("onPause")
    public static void onPause(@ThisObject Activity thiz) throws Throwable {
        Log.e("ActivityHooker", "hooked onPause success " + thiz);
        onPauseBackup.callOrigin(thiz);
    }

}
```

---

#### Xposed API

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

----

## ART Invoke 代码生成

- 前言
- ArtMethod
- Quick & Optimizing
- Optimizing
- Sharpening
- 8.0 之前
- 8.0 之后
- 结论

---

### 前言
在正式聊 Hook 方案之前，我们需要先了解一下 ART 对 invoke 字节码的实现，因为这会决定 Hook 的部分实现。
这里的实现理论上分为：解释器实现和编译实现(JIT/AOT)
实际上解释器的实现比较稳定和单一，我们仅仅需要关注编译器实现即可。

---

### ArtMethod

在了解 ArtMethod 之前先了解一下这个概念：

- Java 对象在内存中的布局可以看成一个结构体，父类的变量在开头，本身的变量紧随其后。
- 这些对象结构体在 ART 中被映射成 mirror::Object cpp 类。
- 有一些虚拟机比较在意的类型，例如 Class，Method，Thread 这些 Art 内部所需要的类型，他们在 mirror 中是有对应的类型的
- 成员变量的内存布局也是对应映射的

---

#### Mirror

- 在大约 6.0 及之前，java 层中有隐藏类 ArtMethod，Method 与之一对一，而 mirror::ArtMethod 就是 java 层 ArtMethod 的映射。
- 6.0 之后，java ArtMethod 不复存在，Method 与 mirror::ArtMethod 一对一映射，只不过大部分 Field 被 "隐藏" 了。

---

#### GC

- ArtMethod 以及类似的 ArtField 在 Linear 堆区，是不会 Moving GC 的。
- 原因很简单，ArtMethod/ArtField 是有可能 JIT/AOT 在 native code 中的，如果随时变化则不好同步。

---

#### jmethodId -> ArtMethod

```cpp
ALWAYS_INLINE
static inline ArtField* DecodeArtField(jfieldID fid) {
  return reinterpret_cast<ArtField*>(fid);
}

ALWAYS_INLINE
static inline jfieldID EncodeArtField(ArtField* field) {
  return reinterpret_cast<jfieldID>(field);
}

ALWAYS_INLINE
static inline jmethodID EncodeArtMethod(ArtMethod* art_method) {
  return reinterpret_cast<jmethodID>(art_method);
}

ALWAYS_INLINE
static inline ArtMethod* DecodeArtMethod(jmethodID method_id) {
  return reinterpret_cast<ArtMethod*>(method_id);
}
```

可以看到只是简单的 cast，jmethodId 是 ArtMethod 的透明引用。

---

#### ArtMethod 结构

```cpp
  // method 所属类，是 GCRoot，Class 类是可以 Moving GC 的
  // 这点需要特别关注，影响实现
  GcRoot<mirror::Class> declaring_class_;
  // java 层的 Modifier 只有其高 16 位
  // 低 16 位用作 ART 的内部运行，在 java 层被隐藏了
  std::atomic<std::uint32_t> access_flags_;
  // 方法的 CodeItem 在 Dex 中的偏移
  uint32_t dex_code_item_offset_;
  // 方法在 Dex 中的 index
  uint32_t dex_method_index_;
  // 虚方法则为实现方法在 VTable 中的 index
  // 非虚方法则是方法在 DexCodeCache 中的 index
  uint16_t method_index_;
  // 方法的热度，JIT 的重要参考
  uint16_t hotness_count_;

  struct PtrSizedFields {
    // 公共存储区域，用不到
    void* data_;
    // 非常重要！
    // 方法的 Code 入口
    // 如果没有编译，则
    // art_quick_to_interpreter_bridge
    // art_quick_generic_jni_trampoline
    // 如果 JIT/AOT 则为编译后的 native ß入口
    void* entry_point_from_quick_compiled_code_;
  } ptr_sized_fields_;

```

---

### Quick & Optimizing
ART 中的 Compiler 有两种 Backend：
- Quick
- Optimizing
<br/>
<font size=4>Quick 在 4.4 就引入，直到 6.0 一直作为默认 Compiler, 直到 7.0 被移除。</font>
<br/>
<font size=4>Optimizing 5.0 引入，7.0 - 9.0 作为唯一 Compiler。</font>
<br/>
<font size=4>还有个叫 portable，基本没用过。。。</font>

下面以 Optimizing Compiler 为例分析 ART 方法调用的生成。


---

### Optimizing
Optimizing比Quick生成速度慢，但是会附带各种优化:
- 逃逸分析：如果不能逃逸，则直接栈上分配
- 常量折叠
- 死代码块移除
- 方法内联
- 指令精简
- 指令重排序
- load/store 精简
- Intrinsic 函数替换 。。。

其中包括 Invoke 代码生成：
---

### Sharpening

**invoke-static/invoke-direct 代码生成默认使用 Sharpening 优化**。

---

#### Sharpening 做了两件事情

- 确定加载 ArtMethod 的方式和位置
- 确定直接 blr 入口调用方法还是查询 ArtMethod -> CodeEntry 调用方法

---

#### 结果保存在两个 enum 中

- MethodLoadKind 就是 ArtMethod 加载类型
- CodePtrLocation 就是跳转地址的类型

我们重点关注 CodePtrLocation，但是 CodePtrLocation 在 8.0 有重大变化。

---

### 8.0 之前

```cpp
 // Determines the location of the code pointer.
  enum class CodePtrLocation {
    // 顾名思义，递归调用自己，此时不需要重新加载 ArtMethod
    // 直接跳转到方法开头
    kCallSelf,
    // 直接 B 到偏移地址，多见于调用附近的方法
    kCallPCRelative,
    // 可以直接知道编译完成的入口代码
    // 则可以跳过 ArtMethod->CodeEntry 查询，直接 blx entry
    // 多见于调用系统方法，这些方法中都是绝对地址，不需要重定向
    kCallDirect,
    // link OAT 文件的时候，才能确定方法在内存中的位置
    // 方法入口需要 linker 重定向，也不需要查询 ArtMethod
    kCallDirectWithFixup,
    // 此种需要在 Runtime 期间得知方法入口
    // 需要查询 ArtMethod->CodeEntry
    // 那么由此可见只有在此种情况下，入口替换的 Hook 才有可能生效
    kCallArtMethod,
  };
```

---

### 代码生成

```cpp
void CodeGeneratorARM64::GenerateStaticOrDirectCall(HInvokeStaticOrDirect* invoke, Location temp) {

//处理 ArtMethod 加载
...........

//生成跳转代码
switch (invoke->GetCodePtrLocation()) {
    case HInvokeStaticOrDirect::CodePtrLocation::kCallSelf:
      __ Bl(&frame_entry_label_);
      break;
    case HInvokeStaticOrDirect::CodePtrLocation::kCallPCRelative: {
      relative_call_patches_.emplace_back(invoke->GetTargetMethod());
      vixl::Label* label = &relative_call_patches_.back().label;
      vixl::SingleEmissionCheckScope guard(GetVIXLAssembler());
      __ Bind(label);
      __ bl(0);  // Branch and link to itself. This will be overriden at link time.
      break;
    }
    case HInvokeStaticOrDirect::CodePtrLocation::kCallDirectWithFixup:
    case HInvokeStaticOrDirect::CodePtrLocation::kCallDirect:
      // LR prepared above for better instruction scheduling.
      DCHECK(direct_code_loaded);
      // lr()
      __ Blr(lr);
      break;
    case HInvokeStaticOrDirect::CodePtrLocation::kCallArtMethod:
      // LR = callee_method->entry_point_from_quick_compiled_code_;
      __ Ldr(lr, MemOperand(
          XRegisterFrom(callee_method),
       ArtMethod::EntryPointFromQuickCompiledCodeOffset(kArm64WordSize).Int32Value()));
      // lr()
      __ Blr(lr);
      break;
  }
}
```

---

#### 汇编

- call self

```asm
_functionxxx:
...
...
bl _functionxxx
```

- call direct

```asm
blr lr
```

- call art method

```asm
ldr lr [RegMethod(X0), #CodeEntryOffset]
blr lr
```

---

### 8.0 之后

或许考虑到真正优化的地方在于如何更快的加载 ArtMethod 结构体，所以 8.0 之后编译后的代码都不会再省略：

```asm
ldr lr [RegMethod(X0), #CodeEntryOffset]
```

这一步。

---

#### CodePtrLocation

```cpp
// Determines the location of the code pointer.
  enum class CodePtrLocation {
    kCallSelf,
    kCallArtMethod,
  };
```

---

#### 代码生成

```cpp
switch (invoke->GetCodePtrLocation()) {
    case HInvokeStaticOrDirect::CodePtrLocation::kCallSelf:
      {
        // Use a scope to help guarantee that `RecordPcInfo()` records the correct pc.
        ExactAssemblyScope eas(GetVIXLAssembler(),
                               kInstructionSize,
                               CodeBufferCheckScope::kExactSize);
        __ bl(&frame_entry_label_);
        RecordPcInfo(invoke, invoke->GetDexPc(), slow_path);
      }
      break;
    case HInvokeStaticOrDirect::CodePtrLocation::kCallArtMethod:
      // LR = callee_method->entry_point_from_quick_compiled_code_;
      __ Ldr(lr, MemOperand(
          XRegisterFrom(callee_method),
          ArtMethod::EntryPointFromQuickCompiledCodeOffset(kArm64PointerSize).Int32Value()));
      {
        // Use a scope to help guarantee that `RecordPcInfo()` records the correct pc.
        ExactAssemblyScope eas(GetVIXLAssembler(),
                               kInstructionSize,
                               CodeBufferCheckScope::kExactSize);
        // lr()
        __ blr(lr);
        RecordPcInfo(invoke, invoke->GetDexPc(), slow_path);
      }
      break;
  }
```

---

### invoke-virtual/interface

<font size=5>调用虚方法并不会使用虚方法的 ArtMethod，因为虚方法本身不含 CodeItem，无法执行。那么调用虚方法则需要从 receiver 的类中的 VTable(虚方法表) 中加载真正的实现方法并且调用。
</font>

```cpp
{
    // Ensure that between load and MaybeRecordImplicitNullCheck there are no pools emitted.
    EmissionCheckScope guard(GetVIXLAssembler(), kMaxMacroInstructionSizeInBytes);
    // /* HeapReference<Class> */ temp = receiver->klass_
    __ Ldr(temp.W(), HeapOperandFrom(LocationFrom(receiver), class_offset));
    MaybeRecordImplicitNullCheck(invoke);
  }
  // Instead of simply (possibly) unpoisoning `temp` here, we should
  // emit a read barrier for the previous class reference load.
  // intermediate/temporary reference and because the current
  // concurrent copying collector keeps the from-space memory
  // intact/accessible until the end of the marking phase (the
  // concurrent copying collector may not in the future).
  GetAssembler()->MaybeUnpoisonHeapReference(temp.W());
  // temp = temp->GetMethodAt(method_offset);
  __ Ldr(temp, MemOperand(temp, method_offset));
  // lr = temp->GetEntryPoint();
  __ Ldr(lr, MemOperand(temp, entry_point.SizeValue()));
  {
    // Use a scope to help guarantee that `RecordPcInfo()` records the correct pc.
    ExactAssemblyScope eas(GetVIXLAssembler(), kInstructionSize, CodeBufferCheckScope::kExactSize);
    // lr();
    __ blr(lr);
    RecordPcInfo(invoke, invoke->GetDexPc(), slow_path);
  }
```

---

#### 伪代码

- Class clazz = receiver.getClass()
- Method method = class.getMethodAt(Index);
- ldr lr method->CodeEntry
- blr lr

---

#### 为何不 Hook Abstract

修改 VTable 是否可行？

---

#### SingleImplementation

```cpp
// Set by the class linker for a method that has only one implementation for a
// virtual call.
static constexpr uint32_t kAccSingleImplementation =  0x08000000;  // method (runtime)
```

##### CHA 优化

```cpp
ArtMethod* single_impl = interface_method->GetSingleImplementation(pointer_size);
if (single_impl == nullptr) {
   // implementation_method becomes the first implementation for
   // interface_method.
   interface_method->SetSingleImplementation(implementation_method, pointer_size);
   // Keep interface_method's single-implementation status.
   return;
}

ArtMethod {
    // Depending on the method type, the data is
    //   - native method: pointer to the JNI function registered to this method
    //                    or a function to resolve the JNI function,
    //   - conflict method: ImtConflictTable,
    //   - abstract/interface method: the single-implementation if any,
    //   - proxy method: the original interface method or constructor,
    //   - other methods: the profiling data.
    void* data_;
}

```

---

##### 优化步骤

CHA 优化属于内联优化

```cpp
  // Try CHA-based devirtualization to change virtual method calls into
  // direct calls.
  // Returns the actual method that resolved_method can be devirtualized to.
  ArtMethod* TryCHADevirtualization(ArtMethod* resolved_method)
    REQUIRES_SHARED(Locks::mutator_lock_);
```

如果 ART 发现是单实现，则将指令修改为 direct calls

---

### InvokeRuntime

一些特殊方法，主要服务于需要在 Runtime 时期才能确定的 Invoke，例如类初始化 <cinit> 函数。(kQuickInitializeType)
InvokeRuntime 会从当前 Thread 中查找 CodeEntry：

---

#### 代码生成

```cpp
void CodeGeneratorARM64::InvokeRuntime(int32_t entry_point_offset,
                                       HInstruction* instruction,
                                       uint32_t dex_pc,
                                       SlowPathCode* slow_path) {
  ValidateInvokeRuntime(instruction, slow_path);
  BlockPoolsScope block_pools(GetVIXLAssembler());
  __ Ldr(lr, MemOperand(tr, entry_point_offset));
  __ Blr(lr);
  RecordPcInfo(instruction, dex_pc, slow_path);
}
```

---

#### 汇编代码

tr 就是线程寄存器，一般 ARM64 是 X19

所以代码出来一般长这样：

```
loc_3e6828:
mov        x0, x19
ldr        x20, [x0, #0x310]
blr        x20
```

---

### Intrinsics 优化

ART 额外维护了一批系统函数的高效实现，这些高效实现利用了CPU的指令，直接跳过了方法调用。

```cpp
  // System.arraycopy.
    case kIntrinsicSystemArrayCopyCharArray:
      return Intrinsics::kSystemArrayCopyChar;

    case kIntrinsicSystemArrayCopy:
      return Intrinsics::kSystemArrayCopy;

    // Thread.currentThread.
    case kIntrinsicCurrentThread:
      return Intrinsics::kThreadCurrentThread;

```

---

#### Thread.currentThread()

```cpp
void IntrinsicCodeGeneratorARM64::VisitThreadCurrentThread(HInvoke* invoke) {
  codegen_->Load(Primitive::kPrimNot, WRegisterFrom(invoke->GetLocations()->Out()),
                 MemOperand(tr, Thread::PeerOffset<8>().Int32Value()));
}
```

最后出来的代码类似这样，直接就把 Thread.nativePeer ldr 给目标寄存器，根本不是方法调用了：

```
ldr x5, [x19, #PeerOffset]
```

---

### 结论

当 8.0 以上时，我们使用 ArtMethod 入口替换即可基本满足 Hook 需求。但如果 8.0 以下，如果不开启 debug 或者 deoptimize 的话，则必须使用 inline hook，否则会漏掉很多调用。

----

## 基本实现

-