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
.assembly test_101_catch {}
.class TestClass{
.method public static void test_101_catch(){
.locals init(int32 locInt101)
  ldc.i4 1
  stloc locInt101
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_101_startTry:
  leave lbl_101_exitSEH
lbl_101_endTry:
lbl_101_startCatch:
  lbl_101_101: nop
  ldstr      "inside catch"
  call       void [mscorlib]System.Console::WriteLine(class System.String)
  leave lbl_101_exitSEH
lbl_101_endCatch:
.try  lbl_101_startTry to lbl_101_endTry catch ['mscorlib']System.Exception handler lbl_101_startCatch to lbl_101_endCatch
lbl_101_exitSEH:
 ldc.i4 1
 ldloc locInt101	
  brtrue  lbl_101_101
  ldstr      "leaving test"
  call       void [mscorlib]System.Console::WriteLine(class System.String)
  pop
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_101_catch()
    .entrypoint
    ret
}
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_000101:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
}
