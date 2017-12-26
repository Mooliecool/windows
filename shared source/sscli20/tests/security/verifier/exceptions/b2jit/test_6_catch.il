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
.assembly test_6_catch {}
.class TestClass{
.method public static void test_6_catch(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_6_startTry:
  lbl_6_1: nop
  newobj     instance void [mscorlib]System.Exception::.ctor()
   throw
  leave lbl_6_exitSEH
lbl_6_endTry:
lbl_6_startCatch:
  ldloc locInt1
   brtrue lbl_6_1
  leave lbl_6_exitSEH
lbl_6_endCatch:
.try  lbl_6_startTry to lbl_6_endTry catch ['mscorlib']System.Exception handler lbl_6_startCatch to lbl_6_endCatch
lbl_6_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_6_catch()
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
