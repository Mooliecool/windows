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
.assembly test_20_try {}
.class TestClass{
.method public static void test_20_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_20_startTry:
	lbl_20_startTry1:
  	ldstr      "inside try"
 	 call       void [mscorlib]System.Console::WriteLine(class System.String) 
	leave lbl_20_exitSEH1	
	lbl_20_endTry1:
  leave lbl_20_exitSEH	
lbl_20_endTry:
lbl_20_startCatch:
	lbl_20_startCatch1:
  	ldstr      "inside catch"
 	 call       void [mscorlib]System.Console::WriteLine(class System.String) 
	leave lbl_20_exitSEH1
	lbl_20_endCatch1:
	lbl_20_exitSEH1:
  leave lbl_20_exitSEH
lbl_20_endCatch:
lbl_20_exitSEH:
.try   lbl_20_startTry1 to  lbl_20_endTry1 catch ['mscorlib']System.Exception handler lbl_20_startCatch1 to  lbl_20_endCatch1
.try   lbl_20_startTry to  lbl_20_endTry catch ['mscorlib']System.Exception handler lbl_20_startCatch to  lbl_20_endCatch
  ldstr      "leaving test"
  call       void [mscorlib]System.Console::WriteLine(class System.String) 
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_20_try()
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
