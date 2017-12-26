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
.assembly test_23_try {}
.class TestClass{
.method public static void test_23_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_23_startTry:
	lbl_23_startTry1:
	  leave lbl_23_startTry
	lbl_23_endTry1:
	lbl_23_startCatch1:
	  leave lbl_23_exitSEH1
	lbl_23_endCatch1:
	lbl_23_exitSEH1:
  leave lbl_23_exitSEH	
lbl_23_endTry:
lbl_23_startCatch:
  leave lbl_23_exitSEH
lbl_23_endCatch:
lbl_23_exitSEH:
.try   lbl_23_startTry1 to  lbl_23_endTry1 catch ['mscorlib']System.Exception handler lbl_23_startCatch to  lbl_23_endCatch
.try   lbl_23_startTry to  lbl_23_endTry catch ['mscorlib']System.Exception handler lbl_23_startCatch to  lbl_23_endCatch
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_23_try()
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
