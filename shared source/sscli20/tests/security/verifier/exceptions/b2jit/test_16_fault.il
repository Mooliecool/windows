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
.assembly test_16_fault {}
.class TestClass{
.method public static void test_16_fault(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_16_startTry
lbl_16_startTry:
  lbl_16_1: nop
  leave lbl_16_exitSEH
lbl_16_endTry:
lbl_16_startFault:
   endfault
lbl_16_endFault:
lbl_16_startFilter1:
  ldc.i4.1
  endfilter
lbl_16_endFilter1: 
lbl_16_startFilterHandler1:
   leave lbl_16_exitSEH
lbl_16_endFilterHandler1:
.try  lbl_16_startTry to lbl_16_endTry fault  handler lbl_16_startFault to lbl_16_endFault
.try  lbl_16_startTry to lbl_16_endTry filter lbl_16_startFilter1  handler lbl_16_startFilterHandler1 to lbl_16_endFilterHandler1
lbl_16_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_16_fault()
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
