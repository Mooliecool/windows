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
// ctor_wrongarg1.il
//
// NEGATIVE
// incorrect method passed in delegate ctor: extra argument
.assembly extern mscorlib { }
.assembly ctor_wrongarg1 { }
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance value class [mscorlib]System.Decimal 
          Invoke(value class [mscorlib]System.Decimal vv) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(value class [mscorlib]System.Decimal vv,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance value class [mscorlib]System.Decimal 
          EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed { }
}
.class public sequential ansi sealed beforefieldinit ValueClass
       extends [mscorlib]System.ValueType
{
  .pack 1
  .size 1
}
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
  .method public hidebysig instance value class [mscorlib]System.Decimal 
          DelegateFunc(value class [mscorlib]System.Decimal vv, int32) il managed
  {
    .maxstack  2
    .locals init (value class [mscorlib]System.Decimal V_0)
    IL_0000:  ldc.i4.s   100
    IL_0002:  newobj     instance void [mscorlib]System.Decimal::.ctor(int32)
    IL_0007:  stloc.0
    IL_0008:  br.s       IL_000a
    IL_000a:  ldloc.0
    IL_000b:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  4
    .locals init (class MyDelegate V_0)
    IL_0000:  newobj     instance void TestClass::.ctor()
    IL_0005:  ldftn      instance value class [mscorlib]System.Decimal TestClass::DelegateFunc(value class [mscorlib]System.Decimal, int32)
    IL_000b:  newobj     instance void MyDelegate::.ctor(object,
                                                         native unsigned int)
    IL_0010:  stloc.0
    IL_0011:  ldc.i4.s   100
    IL_0013:  ldloc.0
    IL_0014:  ldc.i4.0
    IL_0015:  newobj     instance void [mscorlib]System.Decimal::.ctor(int32)
    IL_001a:  callvirt   instance value class [mscorlib]System.Decimal MyDelegate::Invoke(value class [mscorlib]System.Decimal)
    IL_001f:  call       int32 [mscorlib]System.Decimal::op_Explicit(value class [mscorlib]System.Decimal)
    IL_0024:  sub
    IL_0025:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
    IL_002a:  ret
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
