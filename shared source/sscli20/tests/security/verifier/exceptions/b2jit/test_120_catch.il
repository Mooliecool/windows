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
.assembly test_120_catch {}
.class TestClass{
.method public static void test_120_catch(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_120_startTry:
  newobj     instance void [mscorlib]System.ArgumentException::.ctor()
   throw
  leave lbl_120_exitSEH
lbl_120_endTry:
lbl_120_startCatch:
   pop
lbl_120_startCatch1:
   leave lbl_120_exitSEH
lbl_120_endCatch:
   pop
   leave lbl_120_exitSEH
lbl_120_endCatch1:
lbl_120_startCatch2:
   pop
   leave lbl_120_exitSEH
lbl_120_endCatch2:
.try  lbl_120_startTry to lbl_120_endTry catch ['mscorlib']System.Exception handler lbl_120_startCatch to lbl_120_endCatch
.try  lbl_120_startTry to lbl_120_endTry catch ['mscorlib']System.ArgumentException handler lbl_120_startCatch1 to lbl_120_endCatch1
.try  lbl_120_startTry to lbl_120_endTry catch ['mscorlib']System.ArgumentNullException handler lbl_120_startCatch2 to lbl_120_endCatch2
lbl_120_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_120_catch()
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
