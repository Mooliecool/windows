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
// ctor_unrelated4.il
// NEGATIVE
.assembly extern mscorlib { }
.assembly ctor_unrelated4 { }
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance void 
          Invoke(class BaseClass) runtime managed { }
}
.class public auto ansi beforefieldinit BaseClass
       extends [mscorlib]System.Object
{
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  }
}
.class public auto ansi beforefieldinit TestClass
       extends BaseClass
{
  .method public hidebysig static void  DelegateFunc(class BaseClass sender) il managed
  {
    .maxstack  8
    IL_0000:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  3
    .locals init (class MyDelegate V_0)
    ldnull
    ldftn      void TestClass::DelegateFunc(class BaseClass)
    newobj     instance void MyDelegate::.ctor(object, native unsigned int)
    newobj     instance void BaseClass::.ctor()
    callvirt   instance void MyDelegate::Invoke(class BaseClass)
    ret
  }
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void BaseClass::.ctor()
    IL_0006:  ret
  }
}
