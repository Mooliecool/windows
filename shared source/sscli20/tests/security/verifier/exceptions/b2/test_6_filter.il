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
.assembly test_6_ter {}
.class TestClass{
.method public static void test_6_filter(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_6_startTry
lbl_6_startTry:
  leave lbl_6_exitSEH
lbl_6_endTry:
lbl_6_startFilter:
  br  lbl_6_exitSEH
  ldc.i4.1
  endfilter
lbl_6_endFilter:
lbl_6_startFilterHandler:
  lbl_6_1:nop
  pop
  leave lbl_6_exitSEH
lbl_6_endFilterHandler:
.try  lbl_6_startTry to lbl_6_endTry filter lbl_6_startFilter handler lbl_6_startFilterHandler to lbl_6_endFilterHandler
lbl_6_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_6_filter()
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
