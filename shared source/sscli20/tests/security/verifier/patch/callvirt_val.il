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
.assembly callvirt_val {}
.assembly extern mscorlib {}
.class value sealed TestVal extends [mscorlib]System.ValueType {
	.field int32 fld
	.method virtual newslot void VirtCall() {
		.maxstack 8
		ret
	}
}
.class TestClass{
.method public static void callvirt_val(){
.locals init (valuetype TestVal)
.zeroinit
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)
  ldloca.s 0
  callvirt  void TestVal::VirtCall()
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    .entrypoint
    call void TestClass::callvirt_val()
    ret
}
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
}
