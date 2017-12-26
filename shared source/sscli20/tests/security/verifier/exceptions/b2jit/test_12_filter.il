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
.assembly test_12_ter {}
.class TestClass{
.method public static void test_12_filter(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_12_startTry
lbl_12_startTry:
  leave lbl_12_exitSEH
lbl_12_endTry:
lbl_12_startFilter:
  lbl_12_1:pop
  ldc.i4.1
  endfilter
lbl_12_endFilter:
lbl_12_startFilterHandler:
  pop
  leave lbl_12_startTry
  leave lbl_12_exitSEH
lbl_12_endFilterHandler:
.try  lbl_12_startTry to lbl_12_endTry filter lbl_12_startFilter handler lbl_12_startFilterHandler to lbl_12_endFilterHandler
lbl_12_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_12_filter()
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
