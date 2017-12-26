// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
// invoke_tail3.il
//
// POSITIVE
// tail. prefix before Invoke(). No byref arguments. SEH at the calling site
// At the runtime, EEException will be thrown and caught
//
.assembly extern mscorlib { }
.assembly invoke_tail3 { }
.class public sequential ansi sealed beforefieldinit ValueClass
       extends [mscorlib]System.ValueType
{
  .field public float64[] gcref
  .field public float32 scalar
  .method public hidebysig instance value class ValueClass 
          DelegateFunc(value class ValueClass a) il managed
  {
    .maxstack  2
    .locals init (value class ValueClass V_0,
             value class ValueClass V_1)
    IL_0000:  ldloca.s   V_0
    IL_0002:  initobj    ValueClass
    IL_0008:  ldloca.s   V_0
    IL_000a:  ldc.r4     10.
    IL_000f:  stfld      float32 ValueClass::scalar
    IL_0014:  ldloc.0
    IL_0015:  stloc.1
    IL_0016:  br.s       IL_0018
    IL_0018:  ldloc.1
    IL_0019:  ret
  }
}
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance value class ValueClass 
          Invoke(value class ValueClass a) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(value class ValueClass a,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance value class ValueClass 
          EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed { }
}
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
  .method private hidebysig static value class ValueClass 
          DelegateWrapper(class MyDelegate myDeleg) il managed
  {
    .maxstack  2
    .locals init (value class ValueClass V_0,
             value class ValueClass V_1)
    IL_0000:  ldarg.0
    IL_0001:  ldloca.s   V_1
    IL_0003:  initobj    ValueClass
    IL_0009:  ldloc.1
    IL_000a:  tail. callvirt   instance value class ValueClass MyDelegate::Invoke(value class ValueClass)
    IL_0013:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  3
    .locals init (value class ValueClass V_0)
    .try
    {
      IL_0000:  ldloca.s   V_0
      IL_0002:  initobj    ValueClass
      IL_0008:  ldloc.0
      IL_0009:  box        ValueClass
      IL_000e:  ldftn      instance value class ValueClass ValueClass::DelegateFunc(value class ValueClass)
      IL_0014:  newobj     instance void MyDelegate::.ctor(object,
                                                           native unsigned int)
      IL_0019:  call       value class ValueClass TestClass::DelegateWrapper(class MyDelegate)
      IL_001e:  pop
      IL_001f:  ldc.i4.0
      IL_0020:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
      IL_0025:  leave.s    IL_0030
    }
    catch [mscorlib]System.Exception 
    {
      IL_0027:  pop
      IL_0028:  ldc.i4.1
      IL_0029:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
      IL_002e:  leave.s    IL_0030
    }
    IL_0030:  ret
  }
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  }
}
