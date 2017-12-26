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
.assembly test_21_fault {}
.class TestClass{
.method public static void test_21_fault(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_21_startTry
lbl_21_startTry:
  lbl_21_1: nop
  leave lbl_21_exitSEH
lbl_21_endTry:
lbl_21_startFault:
lbl_21_startFault1:
   endfault
lbl_21_endFault:
br lbl_21_startTry1
   endfault
lbl_21_endFault1:
lbl_21_startTry1:
 nop
  leave lbl_21_exitSEH
lbl_21_endTry1:
.try  lbl_21_startTry to lbl_21_endTry fault  handler lbl_21_startFault to lbl_21_endFault
.try  lbl_21_startTry1 to lbl_21_endTry1 fault  handler lbl_21_startFault1 to lbl_21_endFault1
lbl_21_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_21_fault()
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
