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
.assembly tail_ret_mismatch {}
.class TestClass{
.method public static float32 tail_ret_mismatch2(){
	ldc.r4 0
	ret
}
.method public static int32 tail_ret_mismatch(){
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)
  tail.
  call float32 TestClass::tail_ret_mismatch2()
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    .entrypoint
    call int32 TestClass::tail_ret_mismatch()
    pop
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
