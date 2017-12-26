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
.assembly test_26_try {}
.class TestClass{
.method public static void test_26_try(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_26_10
lbl_26_startTry:
  leave lbl_26_exitSEH	
lbl_26_endTry:
lbl_26_startCatch:
lbl_26_10:
  ldstr      "inside catch"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  leave lbl_26_exitSEH
lbl_26_endCatch:
lbl_26_startCatch1:
  ldstr      "inside catch1"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
  leave lbl_26_exitSEH
lbl_26_endCatch1:
lbl_26_exitSEH:
.try   lbl_26_startTry to  lbl_26_endTry catch ['mscorlib']System.ArgumentException handler lbl_26_startCatch to  lbl_26_endCatch
.try   lbl_26_startCatch to  lbl_26_endCatch catch ['mscorlib']System.ArgumentException handler lbl_26_startCatch1 to  lbl_26_endCatch1
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_26_try()
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
