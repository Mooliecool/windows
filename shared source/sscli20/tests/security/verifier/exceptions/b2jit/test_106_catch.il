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
.assembly test_106_catch {}
.class TestClass{
.method public static void test_106_catch(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_106_startTry:
  lbl_106_1: nop
  newobj     instance void [mscorlib]System.Exception::.ctor()
   throw
  leave lbl_106_exitSEH
lbl_106_endTry:
lbl_106_startCatch:
  ldloc locInt1
   brtrue lbl_106_endCatch
  leave lbl_106_exitSEH
lbl_106_endCatch:
.try  lbl_106_startTry to lbl_106_endTry catch ['mscorlib']System.Exception handler lbl_106_startCatch to lbl_106_endCatch
lbl_106_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_106_catch()
    .entrypoint
    ret
}
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_000106:  ret
  } 
}
