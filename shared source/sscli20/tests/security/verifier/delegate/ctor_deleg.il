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
// ctor_deleg.il
//
// POSITIVE
// construct delegate using another delegate
//
.assembly extern mscorlib { }
.assembly ctor_deleg { }
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
  .method public hidebysig static int32[]
          DelegateFunc() il managed
  {
    .maxstack  3
    .locals init (int32[] V_0,
             int32[] V_1)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.Int32
    IL_0006:  stloc.1
    IL_0007:  ldloc.1
    IL_0008:  ldc.i4.0
    IL_0009:  ldc.i4.5
    IL_000a:  stelem.i4
    IL_000b:  ldloc.1
    IL_000c:  stloc.0
    IL_000d:  br.s       IL_000f
    IL_000f:  ldloc.0
    IL_0010:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  8
    ldnull
    ldftn		int32[] TestClass::DelegateFunc()
    newobj		instance void MyDelegate::.ctor(object, native unsigned int)
	dup 
	ldvirtftn	instance int32[] MyDelegate::Invoke()
    newobj		instance void MyDelegate::.ctor(object, native unsigned int)
	dup 
	ldvirtftn	instance int32[] MyDelegate::Invoke()
    newobj		instance void MyDelegate::.ctor(object, native unsigned int)
	dup 
	ldvirtftn	instance int32[] MyDelegate::Invoke()
    newobj		instance void MyDelegate::.ctor(object, native unsigned int)
	dup 
	ldvirtftn	instance int32[] MyDelegate::Invoke()
    newobj		instance void MyDelegate::.ctor(object, native unsigned int)
    callvirt   instance int32[] MyDelegate::Invoke()
    ldc.i4.0
    ldc.i4.0
    stelem.i4
    ret
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
