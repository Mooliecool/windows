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
.assembly test_11_ter {}
.class TestClass{
.method public static void test_11_filter(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_11_startTry
lbl_11_startTry:
  leave lbl_11_exitSEH
lbl_11_endTry:
lbl_11_startFilter:
  lbl_11_1:nop
  ldc.i4.1
  endfilter
lbl_11_endFilter:
lbl_11_startFilterHandler:
  br lbl_11_1
  pop
  leave lbl_11_exitSEH
lbl_11_endFilterHandler:
.try  lbl_11_startTry to lbl_11_endTry filter lbl_11_startFilter handler lbl_11_startFilterHandler to lbl_11_endFilterHandler
lbl_11_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_11_filter()
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
