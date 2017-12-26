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
// call_ctor11.il
//
// NEGATIVE
// delegate sign != method sign
.assembly extern mscorlib { }
.assembly call_ctor11 { }
.class private auto ansi sealed MyDelegate
       extends [mscorlib]System.MulticastDelegate
{
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor(object 'object',
                               native unsigned int 'method') runtime managed { }
  .method public hidebysig virtual instance class [mscorlib]System.Object
          Invoke(int64,class System.String) runtime managed { }
  .method public hidebysig newslot virtual 
          instance class [mscorlib]System.IAsyncResult 
          BeginInvoke(int64,class System.String,
                      class [mscorlib]System.AsyncCallback callback,
                      object 'object') runtime managed { }
  .method public hidebysig newslot virtual void 
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
  .method public hidebysig  instance class [mscorlib]System.Object
          DelegateFunc(int32,class System.String) il managed
  {
 	ldstr      "Inside delegate ..."
  	call       void [mscorlib]System.Console::WriteLine(class System.String)  
	newobj     instance  void [mscorlib]System.Object::.ctor()
	ret
  }
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  4
    .locals init (class MyDelegate V_0)
   	  newobj     instance void TestClass::.ctor()
    IL_0005:  ldftn  instance  class [mscorlib]System.Object TestClass::DelegateFunc(int32,class System.String)
    IL_000b:  newobj     instance void  MyDelegate::.ctor(object,
                                                         native unsigned int)
	ldc.i8 0
        ldstr      ""
    IL_001a:  call   instance class [mscorlib]System.Object MyDelegate::Invoke(int64,class System.String)
	pop
	ldstr      "Test finished"
  	call       void [mscorlib]System.Console::WriteLine(class System.String) 
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
.class public auto ansi beforefieldinit TestClass1
       extends [mscorlib]System.Object
{
  .method public hidebysig virtual instance  void
          DelegateFunc() il managed
  {
 	ldstr      "Inside delegate 1..."
  	call       void [mscorlib]System.Console::WriteLine(class System.String)  
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
