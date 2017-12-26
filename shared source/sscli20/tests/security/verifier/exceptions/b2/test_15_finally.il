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
.assembly test_15_finally {}
.class TestClass{
.method public static void test_15_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_15_startTry:
  lbl_15_1: nop
  leave lbl_15_exitSEH
lbl_15_endTry:
lbl_15_startFinally:
   endfinally
lbl_15_endFinally:
lbl_15_startFinally1:
   endfinally
lbl_15_endFinally1:
.try  lbl_15_startTry to lbl_15_endTry finally  handler lbl_15_startFinally to lbl_15_endFinally
.try  lbl_15_startTry to lbl_15_endTry finally  handler lbl_15_startFinally1 to lbl_15_endFinally1
lbl_15_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_15_finally()
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
