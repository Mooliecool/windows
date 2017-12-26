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
.assembly stack_u {}
.class TestClass{
.field static int32 a
.method public static void stack_u(int32 a){
.locals init(int32 a)
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String) 
        stsfld int32 TestClass::a
	ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    ldc.i4.1
    call void TestClass::stack_u(int32)
    .entrypoint
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