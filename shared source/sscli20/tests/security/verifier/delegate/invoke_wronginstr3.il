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
// invoke_wronginstr3.il
//
// NEGATIVE
// attempt to invoke delegate using ldftn+calli
.assembly extern mscorlib { }
.assembly invoke_wronginstr3 { }
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance class TestClass[] 
          Invoke(bool good,
                 class TestClass& inst) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(bool good,
                      class TestClass& inst,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance class TestClass[]  EndInvoke(class TestClass& inst,
                                                class [mscorlib]System.IAsyncResult result) runtime managed { }
}
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
  .method public hidebysig instance class TestClass[] 
          DelegateFunc(bool good,
                       class TestClass& inst) il managed
  {
    .maxstack  3
    .locals init (class TestClass[] V_0,
             class TestClass[] V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     TestClass
    IL_0006:  stloc.1
    IL_0007:  ldloc.1
    IL_0008:  ldc.i4.0
    IL_0009:  newobj     instance void TestClass::.ctor()
    IL_000e:  stelem.ref
    IL_000f:  ldloc.1
    IL_0010:  stloc.0
    IL_0011:  br.s       IL_0013
    IL_0013:  ldloc.0
    IL_0014:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  4
    .locals init (class MyDelegate V_0,
             class TestClass V_1)
    IL_0000:  newobj     instance void TestClass::.ctor()
    IL_0005:  ldftn      instance class TestClass[] TestClass::DelegateFunc(bool,
                                                                            class TestClass&)
    IL_000b:  newobj     instance void MyDelegate::.ctor(object,
                                                         native unsigned int)
    IL_0010:  stloc.0
    IL_0011:  ldnull
    IL_0012:  stloc.1
    IL_0013:  ldloc.0
    IL_0014:  ldc.i4.0
    IL_0015:  ldloca.s   V_1
    IL_0017:  ldftn instance class TestClass[] MyDelegate::Invoke(bool, class TestClass&)
			  calli instance class TestClass[](bool, class TestClass&)
    IL_001c:  pop
    IL_001d:  ret
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
