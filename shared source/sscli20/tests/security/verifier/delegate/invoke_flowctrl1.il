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
// invoke_flowctrl1.il
//
// POSITIVE
// merge immediately before Invoke()
.assembly extern mscorlib { }
.assembly invoke_flowctrl1 { }
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance value class ValueClass 
          Invoke(value class ValueClass& vv) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(value class ValueClass& vv,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual 
          instance value class ValueClass 
          EndInvoke(value class ValueClass& vv,
                    class [mscorlib]System.IAsyncResult result) runtime managed { }
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
  .method public hidebysig instance value class ValueClass 
          DelegateFunc(value class ValueClass& vv) il managed
  {
    .maxstack  1
    .locals init (value class ValueClass V_0,
             value class ValueClass V_1)
    IL_0000:  ldloca.s   V_1
    IL_0002:  initobj    ValueClass
    IL_0008:  ldloc.1
    IL_0009:  stloc.0
    IL_000a:  br.s       IL_000c
    IL_000c:  ldloc.0
    IL_000d:  ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  8
    .locals init (class MyDelegate V_0,
             value class ValueClass[] V_1,
             int32 SELECTOR)
	ldc.i4 896
	ldc.i4.7
	rem
    stloc SELECTOR
    newobj     instance void TestClass::.ctor()
    ldftn      instance value class ValueClass TestClass::DelegateFunc(value class ValueClass&)
	ldloc SELECTOR
	ldc.i4.3
	rem
	brfalse BRANCH_B
BRANCH_A:
    newobj     instance void MyDelegate::.ctor(object, native unsigned int)
    stloc.0
    ldc.i4.2
    newarr     ValueClass
    stloc.1
    ldloc.1
    ldc.i4.1
	ldloc SELECTOR
	ldc.i4.2
	rem
	brtrue MERGE_B
    ldelema    ValueClass
    ldloc.0
    ldloc.1
    ldc.i4.0
    ldelema    ValueClass
    br.s MERGE
BRANCH_B:    
    newobj     instance void MyDelegate::.ctor(object, native unsigned int)
    stloc.0
    ldc.i4.2
    newarr     ValueClass
    stloc.1
    ldloc.1
    ldc.i4.1
MERGE_B:
    ldelema    ValueClass
    ldloc.0
    ldloc.1
    ldc.i4.0
    ldelema    ValueClass
    dup
    dup
    brfalse THR
    pop
MERGE:
    callvirt   instance value class ValueClass MyDelegate::Invoke(value class ValueClass&)
    stobj      ValueClass
    ret
THR:
	newobj instance void [mscorlib]System.ApplicationException::.ctor()
	throw
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
