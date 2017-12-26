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
.assembly test_27_try {}
.class TestClass{
.method public static void test_27_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_27_startTry:
  nop
	lbl_27_startTry1:
 	 ldstr      "inside try"
 	 call       void [mscorlib]System.Console::WriteLine(class System.String)  
	  leave lbl_27_exitSEH
	lbl_27_endTry1:
	lbl_27_startCatch1:
	  leave lbl_27_exitSEH1
	lbl_27_endCatch1:
	lbl_27_exitSEH1:
  leave lbl_27_exitSEH	
lbl_27_endTry:
lbl_27_startCatch:
  leave lbl_27_exitSEH
lbl_27_endCatch:
lbl_27_exitSEH:
.try   lbl_27_startTry to  lbl_27_endTry1 catch ['mscorlib']System.ArgumentException handler lbl_27_startCatch1 to  lbl_27_endCatch1
.try   lbl_27_startTry1 to  lbl_27_endTry catch ['mscorlib']System.ArgumentException handler lbl_27_startCatch to  lbl_27_endCatch
 ldstr      "leaving test"
  call       void [mscorlib]System.Console::WriteLine(class System.String) 
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_27_try()
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
