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
// ctor_wrongarg2.il
//
// NEGATIVE
// incorrect method passed in delegate ctor: mismatched argument types
.assembly extern mscorlib { }
.assembly ctor_wrongarg2 { }
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance float32 
          Invoke(value class [mscorlib]System.Decimal vv) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(value class [mscorlib]System.Decimal vv,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance float32  EndInvoke(class [mscorlib]System.IAsyncResult result) runtime managed { }
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
  .method public hidebysig instance float64 
          DelegateFunc(value class [mscorlib]System.Decimal vv) il managed
  {
    .maxstack  1
    .locals init (float32 V_0)
    IL_0000:  ldc.r4     100.
    IL_0005:  stloc.0
    IL_0006:  br.s       IL_0008
    IL_0008:  ldloc.0
    IL_0009:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  4
    .locals init (class MyDelegate V_0)
    IL_0000:  newobj     instance void TestClass::.ctor()
    IL_0005:  ldftn      instance float64 TestClass::DelegateFunc(value class [mscorlib]System.Decimal)
    IL_000b:  newobj     instance void MyDelegate::.ctor(object,
                                                         native unsigned int)
    IL_0010:  stloc.0
    IL_0011:  ldc.i4.s   100
    IL_0013:  ldloc.0
    IL_0014:  ldc.i4.0
    IL_0015:  newobj     instance void [mscorlib]System.Decimal::.ctor(int32)
    IL_001a:  callvirt   instance float32 MyDelegate::Invoke(value class [mscorlib]System.Decimal)
    IL_001f:  conv.i4
    IL_0020:  sub
    IL_0021:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
    IL_0026:  ret
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
