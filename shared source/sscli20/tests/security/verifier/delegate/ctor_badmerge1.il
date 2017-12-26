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
// ctor_badmerge1.il
//
// NEGATIVE
// merge of two unrelated delegates
.assembly extern mscorlib { }
.assembly ctor_badmerge1 { }
.class private auto ansi sealed MyDelegate1
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance unsigned int64 
          Invoke(class System.String a) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(class System.String a,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance unsigned int64  EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed { }
}
.class private auto ansi sealed MyDelegate2
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance unsigned int64 
          Invoke(class System.String a) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(class System.String a,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance unsigned int64  EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed { }
}
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
  .field private static bool selector
  .method private hidebysig static unsigned int64 
          DelegateFunc1(class System.String str) il managed
  {
    .maxstack  1
    .locals init (unsigned int64 V_0)
    IL_0000:  ldarg.0
    IL_0001:  call       void [mscorlib]System.Console::WriteLine(class System.String)
    IL_0006:  ldc.i4.s   10
    IL_0008:  conv.i8
    IL_0009:  stloc.0
    IL_000a:  br.s       IL_000c
    IL_000c:  ldloc.0
    IL_000d:  ret
  }
  .method private hidebysig static unsigned int64 
          DelegateFunc2(class System.String str) il managed
  {
    .maxstack  1
    .locals init (unsigned int64 V_0)
    IL_0000:  ldarg.0
    IL_0001:  call       void [mscorlib]System.Console::WriteLine(class System.String)
    IL_0006:  ldc.i8   10
    IL_0008:  stloc.0
    IL_0009:  br.s       IL_000b
    IL_000b:  ldloc.0
    IL_000c:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  4
    .locals init (class MyDelegate1 V_0)
    ldsfld     bool TestClass::selector
    brtrue.s   IL_0015
    ldnull
    ldftn      unsigned int64 TestClass::DelegateFunc1(class System.String)
    newobj     instance void MyDelegate1::.ctor(object,
                                                          native unsigned int)
    br.s       IL_0021
    IL_0015:  ldnull
    ldftn      unsigned int64 TestClass::DelegateFunc2(class System.String)
    newobj     instance void MyDelegate2::.ctor(object,
                                                          native unsigned int)
    IL_0021:  stloc.0
    IL_0022:  ldloc.0
    IL_0023:  ldstr      "Hello!"
    IL_0028:  callvirt   instance unsigned int64 MyDelegate1::Invoke(class System.String)
    IL_002d:  ldc.i4.s   10
    IL_002f:  conv.i8
    IL_0030:  beq.s      IL_0035
    IL_0032:  ldc.i4.1
    IL_0033:  br.s       IL_0036
    IL_0035:  ldc.i4.0
    IL_0036:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
    IL_003b:  ret
  }
  .method private hidebysig specialname rtspecialname static 
          void .cctor() il managed
  {
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  stsfld     bool TestClass::selector
    IL_0006:  ret
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
