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
.assembly test_21_catch {}
.class TestClass{
.method public static void test_21_catch(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_21_startTry:
  leave lbl_21_exitSEH
lbl_21_endTry:
lbl_21_startCatch:
  pop
  lbl_21_startTry1:
    leave lbl_21_exitSEH1
  lbl_21_endTry1:
  lbl_21_startCatch1:
     leave lbl_21_exitSEH1
  lbl_21_endCatch1:
lbl_21_exitSEH1:
   leave lbl_21_exitSEH
lbl_21_endCatch:
.try  lbl_21_startTry1 to lbl_21_endTry1 catch ['mscorlib']System.Exception handler lbl_21_startCatch1 to lbl_21_endCatch1
.try  lbl_21_startTry to lbl_21_endTry catch ['mscorlib']System.Exception handler lbl_21_startCatch to lbl_21_endCatch
lbl_21_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_21_catch()
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
