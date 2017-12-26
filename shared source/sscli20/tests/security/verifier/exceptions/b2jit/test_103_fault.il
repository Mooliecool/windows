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
.assembly test_103_fault {}
.class TestClass{
.method public static void test_103_fault(){
.locals init(int32 locInt1)
lbl_103_UP:
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_103_startTry:
  leave lbl_103_exitSEH
lbl_103_endTry:
lbl_103_startFault:
  lbl_103_1: nop
  br lbl_103_UP
  endfault
lbl_103_endFault:
.try  lbl_103_startTry to lbl_103_endTry fault  handler lbl_103_startFault to lbl_103_endFault
lbl_103_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_103_fault()
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
