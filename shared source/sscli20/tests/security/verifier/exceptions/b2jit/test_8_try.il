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
.assembly test_8_try {}
.class TestClass{
.method public static void test_8_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_8_startTry:
  ldstr      "inside try"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  br  lbl_8_exitSEH 	
  leave lbl_8_exitSEH
lbl_8_endTry:
lbl_8_startCatch:
  ldstr      "inside catch"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  leave lbl_8_exitSEH
lbl_8_endCatch:
.try  lbl_8_startTry to lbl_8_endTry catch ['mscorlib']System.Exception handler lbl_8_startCatch to lbl_8_endCatch
lbl_8_exitSEH:
  ldstr      "leaving test"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_8_try()
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
