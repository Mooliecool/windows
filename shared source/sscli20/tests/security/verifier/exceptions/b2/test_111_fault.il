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
.assembly test_111_fault {}
.class TestClass{
.method public static void test_111_fault(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_11_startTry:
  lbl_11_1: nop
  ldstr      "inside try1"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
   ldc.i4.1
   pop
lbl_11_startTry1:
  lbl_11_2: nop
  ldstr      "inside try2"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
   	newobj     instance void [mscorlib]System.Exception::.ctor()
   	throw
  leave lbl_11_exitSEH
lbl_11_endTry:
  leave lbl_11_exitSEH
lbl_11_endTry1:
lbl_11_startFault:
  ldstr      "inside endfault1"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
   endfault
lbl_11_endFault:
lbl_11_startFault1:
  ldstr      "inside endfault2"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
   endfault
lbl_11_endFault1:
.try  lbl_11_startTry to lbl_11_endTry fault  handler lbl_11_startFault to lbl_11_endFault
.try  lbl_11_startTry1 to lbl_11_endTry1 fault  handler lbl_11_startFault1 to lbl_11_endFault1
lbl_11_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_111_fault()
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
