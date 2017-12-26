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
.assembly test_10_finally {}
.class TestClass{
.method public static void test_10_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_10_startTry1:
  br lbl_10_startTry
  lbl_10_startFinally:
     nop 
     endfinally
  lbl_10_endFinally:
  lbl_10_startTry:
    lbl_10_1: nop
    leave lbl_10_exitSEH
  lbl_10_endTry:
lbl_10_endTry1:
lbl_10_startCatch1:
  leave lbl_10_exitSEH
lbl_10_endCatch1:
.try  lbl_10_startTry to lbl_10_endTry finally  handler lbl_10_startFinally to lbl_10_endFinally
.try  lbl_10_startTry1 to lbl_10_endTry1 catch ['mscorlib']System.Exception  handler lbl_10_startCatch1 to lbl_10_endCatch1
lbl_10_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_10_finally()
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
