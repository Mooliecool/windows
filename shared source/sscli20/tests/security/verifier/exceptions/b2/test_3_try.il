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
.assembly test_3_try {}
.class TestClass{
.method public static void test_3_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  ldloc locInt1	
  brtrue  lbl_3_1
lbl_3_startTry:
  lbl_3_1: nop
  leave lbl_3_exitSEH
lbl_3_endTry:
lbl_3_startCatch:
  pop
  leave lbl_3_exitSEH
lbl_3_endCatch:
.try  lbl_3_startTry to lbl_3_endTry catch ['mscorlib']System.Exception handler lbl_3_startCatch to lbl_3_endCatch
lbl_3_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_3_try()
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
