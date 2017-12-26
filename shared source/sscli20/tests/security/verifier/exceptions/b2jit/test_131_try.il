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
.assembly test_131_try {}
.class TestClass{
.method public static void test_131_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_131_startTry:
	   newobj     instance void [mscorlib]System.Exception::.ctor()
	   throw	
	lbl_131_startTry1:
	 ldstr      "inside try1"
  	call       void [mscorlib]System.Console::WriteLine(class System.String) 
	  leave lbl_131_exitSEH
	lbl_131_endTry1:
	lbl_131_startCatch1:
       	nop
	lbl_131_1:
	 ldstr      "inside catch1"
  	call       void [mscorlib]System.Console::WriteLine(class System.String)
	  leave lbl_131_exitSEH1
	lbl_131_endCatch1:
	lbl_131_exitSEH1:
  leave lbl_131_exitSEH	
lbl_131_endTry:
lbl_131_startCatch:
	 ldstr      "inside catch"
  	call       void [mscorlib]System.Console::WriteLine(class System.String) 
  leave lbl_131_1
lbl_131_endCatch:
lbl_131_exitSEH:
.try   lbl_131_startTry1 to  lbl_131_endTry1 catch ['mscorlib']System.ArgumentException handler lbl_131_startCatch1 to  lbl_131_endCatch1
.try   lbl_131_startTry to  lbl_131_endTry catch ['mscorlib']System.Exception handler lbl_131_startCatch to  lbl_131_endCatch
	 ldstr      "leaving test"
  	call       void [mscorlib]System.Console::WriteLine(class System.String) 
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_131_try()
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
