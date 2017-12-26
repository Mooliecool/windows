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
.assembly test_18_try {}
.class TestClass{
.method public static void test_18_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_18_startTry:
	lbl_18_startTry1:
 	 ldstr      "inside try"
  	call       void [mscorlib]System.Console::WriteLine(class System.String)
	leave lbl_18_exitSEH1	
	lbl_18_endTry1:
	lbl_18_startCatch1:
 	 ldstr      "inside catch"
 	 call       void [mscorlib]System.Console::WriteLine(class System.String)
	leave lbl_18_exitSEH1
	lbl_18_endCatch1:
	lbl_18_exitSEH1:
  leave lbl_18_exitSEH	
lbl_18_endTry:
lbl_18_startCatch:
  leave lbl_18_exitSEH
lbl_18_endCatch:
lbl_18_exitSEH:
.try   lbl_18_startTry1 to  lbl_18_endTry1 catch ['mscorlib']System.Exception handler lbl_18_startCatch1 to  lbl_18_endCatch1
.try   lbl_18_startTry to  lbl_18_endTry catch ['mscorlib']System.Exception handler lbl_18_startCatch to  lbl_18_endCatch
  ldstr      "leaving test"
  call       void [mscorlib]System.Console::WriteLine(class System.String)
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_18_try()
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
