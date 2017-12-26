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
.assembly test_4_fault{}
.class public auto ansi TestClass extends ['mscorlib']System.Object {
.method public static void test_4_fault(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "start test ..."
  call       void [mscorlib]System.Console::WriteLine(class System.String) 
lbl_4_startTry:
  ldstr      "before throw"
  call       void [mscorlib]System.Console::WriteLine(class System.String) 
   newobj     instance void [mscorlib]System.Exception::.ctor()
   throw
  leave lbl_4_exitSEH
lbl_4_endTry:
lbl_4_startFault:
  lbl_4_1: nop
   ldstr      "in fault"
  call       void [mscorlib]System.Console::WriteLine(class System.String) 
lbl_4_endFault:
.try  lbl_4_startTry to lbl_4_endTry fault  handler lbl_4_startFault to lbl_4_endFault
lbl_4_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    .entrypoint
call void TestClass::test_4_fault()
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
