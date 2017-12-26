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
.assembly test_40_try {}
.class TestClass{
.method public static void test_40_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  ldloc locInt1	
  brtrue  lbl_40_1
lbl_40_startTry:
  lbl_40_1: nop
  leave lbl_40_exitSEH
lbl_40_endTry:
lbl_40_startCatch:
  ldc.i4 1
  pop
  leave lbl_40_exitSEH
lbl_40_endCatch:
lbl_40_exitSEH:
.try  lbl_40_startTry to lbl_40_endTry catch ['mscorlib']System.Exception handler lbl_40_startCatch to lbl_40_endCatch
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_40_try()
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
