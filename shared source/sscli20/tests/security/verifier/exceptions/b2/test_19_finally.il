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
.assembly test_19_finally {}
.class TestClass{
.method public static void test_19_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_19_startTry:
  lbl_19_1: nop
  leave lbl_19_exitSEH
lbl_19_endTry:
lbl_19_startFinally:
   tail. 
   call void TestClass::foo_19_finally()
   endfinally
lbl_19_endFinally:
.try  lbl_19_startTry to lbl_19_endTry finally  handler lbl_19_startFinally to lbl_19_endFinally
lbl_19_exitSEH:
  ret
}
.method public static void foo_19_finally(){
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_19_finally()
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
