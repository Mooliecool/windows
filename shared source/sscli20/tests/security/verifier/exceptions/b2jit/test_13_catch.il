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
.assembly test_13_catch {}
.class TestClass{
.method public static void test_13_catch(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_13_startTry:
 ldstr "ExpMsg"
 newobj     instance void [mscorlib]System.ArgumentException::.ctor(string)
   throw
  leave lbl_13_exitSEH
lbl_13_endTry:
lbl_13_startCatch:
  ldstr      "inside catch"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
	call       void [mscorlib]System.Console::WriteLine(object)
	lbl_13_startTry_1:
	  ldstr      "inside try1"
	  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  		rethrow
	  leave lbl_13_exitSEH_1
	lbl_13_endTry_1:
	lbl_13_startCatch_1:
 	ldstr      "inside catch1"
  	call       void [mscorlib]System.Console::WriteLine(class System.String)  
	call       void [mscorlib]System.Console::WriteLine(object)
  	leave lbl_13_exitSEH_1
	lbl_13_endCatch_1:
lbl_13_exitSEH_1:
  leave lbl_13_exitSEH
lbl_13_endCatch:
.try  lbl_13_startTry_1 to lbl_13_endTry_1 catch ['mscorlib']System.ArgumentException handler lbl_13_startCatch_1 to lbl_13_endCatch_1
.try  lbl_13_startTry to lbl_13_endTry catch ['mscorlib']System.ArgumentException handler lbl_13_startCatch to lbl_13_endCatch
lbl_13_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_13_catch()
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
