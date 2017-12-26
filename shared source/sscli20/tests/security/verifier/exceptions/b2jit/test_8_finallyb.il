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
.assembly test_8_finally {}
.class TestClass{
.method public static void test_8_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_8_startTry:
  lbl_8_1: nop
           newobj     instance void [mscorlib]System.Exception::.ctor()
   	   throw
           leave lbl_8_exitSEH
lbl_8_endTry:
lbl_8_startCatch:
        pop
	leave lbl_8_exitSEH_1
lbl_tryStart:
   	newobj     instance void [mscorlib]System.Exception::.ctor()
   	throw
lbl_8_exitSEH_1:
.try lbl_tryStart to lbl_8_exitSEH_1 catch ['mscorlib']System.Exception handler lbl_8_startCatch to lbl_tryStart
        leave lbl_8_exitSEH
lbl_8_endCatch:
.try  lbl_8_startTry to lbl_8_endTry catch ['mscorlib']System.Exception handler lbl_8_startCatch to lbl_8_endCatch
lbl_8_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_8_finally()
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
