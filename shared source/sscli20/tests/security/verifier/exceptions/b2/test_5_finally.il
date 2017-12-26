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
.assembly test_5_finally {}
.class TestClass{
.method public static void test_5_finally(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_5_startTry:
  leave lbl_5_exitSEH
lbl_5_endTry:
lbl_5_startFinally:
   ldc.i4.1
   pop
   endfilter
lbl_5_endFinally:
.try  lbl_5_startTry to lbl_5_endTry finally  handler lbl_5_startFinally to lbl_5_endFinally
lbl_5_exitSEH:
ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_5_finally()
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
