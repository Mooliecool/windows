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
// ctor_merge2.il
//
// NEGATIVE
// merge of instance and static method
.assembly extern mscorlib { }
.assembly ctor_merge2 { }
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance int32[]
          Invoke() runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance int32[]  EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed { }
}
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
  .field private static bool _sel
  .method public hidebysig static int32[]
          DelegateFunc1() il managed
  {
    .maxstack  3
    .locals init (int32[] V_0,
             int32[] V_1)
    IL_0000:  ldstr      "DelegateFunc1"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class System.String)
    IL_000a:  ldc.i4.1
    IL_000b:  newarr     [mscorlib]System.Int32
    IL_0010:  stloc.1
    IL_0011:  ldloc.1
    IL_0012:  ldc.i4.0
    IL_0013:  ldc.i4.5
    IL_0014:  stelem.i4
    IL_0015:  ldloc.1
    IL_0016:  stloc.0
    IL_0017:  br.s       IL_0019
    IL_0019:  ldloc.0
    IL_001a:  ret
  }
  .method public hidebysig instance int32[]
          DelegateFunc2() il managed
  {
    .maxstack  1
    .locals init (int32[] V_0)
    IL_0000:  ldstr      "DelegateFunc2"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class System.String)
    IL_000a:  ldnull
    IL_000b:  stloc.0
    IL_000c:  br.s       IL_000e
    IL_000e:  ldloc.0
    IL_000f:  ret
  }
  .method private hidebysig static bool sel() il managed
  {
    .maxstack  2
    .locals init (bool V_0)
    IL_0000:  ldsfld     bool TestClass::_sel
    IL_0005:  ldc.i4.0
    IL_0006:  ceq
    IL_0008:  stsfld     bool TestClass::_sel
    IL_000d:  ldsfld     bool TestClass::_sel
    IL_0012:  stloc.0
    IL_0013:  br.s       IL_0015
    IL_0015:  ldloc.0
    IL_0016:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  4
    newobj     instance void TestClass::.ctor()
    call       bool TestClass::sel()
    brtrue.s   BRANCH
    ldftn      instance int32[] TestClass::DelegateFunc2()
    br.s       MERGE
BRANCH:
    ldftn      int32[] TestClass::DelegateFunc1()
MERGE:
    newobj     instance void MyDelegate::.ctor(object, native unsigned int)
    callvirt   instance int32[] MyDelegate::Invoke()
    ldc.i4.0
    ldc.i4.0
    stelem.i4
    ret
  }
  .method private hidebysig specialname rtspecialname static 
          void .cctor() il managed
  {
    .maxstack  8
    IL_0000:  ldc.i4.0
    IL_0001:  stsfld     bool TestClass::_sel
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
