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
// invoke_wrongsig1.il
.assembly extern mscorlib { }
.assembly invoke_wrongsig1 { }
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance unsigned int64 
          Invoke(float64 sc,
                 class TestClass[] ar) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(float64 sc,
                      class TestClass[] ar,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance unsigned int64  EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed { }
}
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
  .method public hidebysig static unsigned int64 
          DelegateFunc(float64 sc,
                       class TestClass[] ar) il managed
  {
    .maxstack  1
    .locals init (unsigned int64 V_0)
    IL_0000:  ldc.i4.s   10
    IL_0002:  conv.i8
    IL_0003:  stloc.0
    IL_0004:  br.s       IL_0006
    IL_0006:  ldloc.0
    IL_0007:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  6
    .locals init (class MyDelegate V_0,
             class TestClass[] V_1)
    IL_0000:  ldnull
    IL_0001:  ldftn      unsigned int64 TestClass::DelegateFunc(float64,
                                                                class TestClass[])
    IL_0007:  newobj     instance void MyDelegate::.ctor(object,
                                                         native unsigned int)
    IL_000c:  stloc.0
    IL_000d:  ldc.i4.s   10
    IL_000f:  conv.i8
    IL_0010:  ldloc.0
    IL_0011:  ldc.r8     0.
    IL_001a:  ldc.i4.1
    IL_001b:  newarr     TestClass
    IL_0020:  stloc.1
    IL_0021:  ldloc.1
    IL_0022:  ldc.i4.0
    IL_0023:  newobj     instance void TestClass::.ctor()
    IL_0028:  stelem.ref
    IL_0029:  ldloc.1
    IL_002a:  callvirt   instance unsigned int64 MyDelegate::Invoke(float32,
                                                                    class TestClass[])
    IL_002f:  sub
    IL_0030:  conv.i4
    IL_0031:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
    IL_0036:  ret
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
