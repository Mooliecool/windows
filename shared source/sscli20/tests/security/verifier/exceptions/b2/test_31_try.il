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
.assembly test_31_try {}
.class TestClass{
.method public static void test_31_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_31_startTry:
	lbl_31_startTry1:
	  nop
          lbl_31_1:
	    nop  
	  leave lbl_31_exitSEH
	lbl_31_endTry1:
	lbl_31_startCatch1:
	  leave lbl_31_exitSEH1
	lbl_31_endCatch1:
	lbl_31_exitSEH1:
  leave lbl_31_exitSEH	
lbl_31_endTry:
lbl_31_startCatch:
  leave lbl_31_1
lbl_31_endCatch:
lbl_31_exitSEH:
.try   lbl_31_startTry1 to  lbl_31_endTry1 catch ['mscorlib']System.Exception handler lbl_31_startCatch1 to  lbl_31_endCatch1
.try   lbl_31_startTry to  lbl_31_endTry catch ['mscorlib']System.Exception handler lbl_31_startCatch to  lbl_31_endCatch
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_31_try()
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
