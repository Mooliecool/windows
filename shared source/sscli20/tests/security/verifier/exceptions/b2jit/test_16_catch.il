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
.assembly test_16_catch {}
.class TestClass{
.method public static void test_16_catch(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_16_startTry:
  leave lbl_16_exitSEH
lbl_16_endTry:
lbl_16_startCatch:
   pop
   tail. call void TestClass::foo_16_catch()
  ret
  leave lbl_16_exitSEH
lbl_16_endCatch:
.try  lbl_16_startTry to lbl_16_endTry catch ['mscorlib']System.Exception handler lbl_16_startCatch to lbl_16_endCatch
lbl_16_exitSEH:
  ret
}
.method public static void foo_16_catch(){
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_16_catch()
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
