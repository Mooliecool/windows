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
.assembly test_37_try {}
.class TestClass{
.method public static void test_37_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_37_startTry:
   tail. call void TestClass::foo_37_try()
   ret
  leave lbl_37_exitSEH
lbl_37_endTry:
lbl_37_startCatch:
   pop
  leave lbl_37_exitSEH
lbl_37_endCatch:
.try  lbl_37_startTry to lbl_37_endTry catch ['mscorlib']System.Exception handler lbl_37_startCatch to lbl_37_endCatch
lbl_37_exitSEH:
  ret
}
.method public static void foo_37_try(){
ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_37_try()
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
