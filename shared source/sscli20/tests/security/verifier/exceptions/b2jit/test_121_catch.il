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
.assembly test_121_catch {}
.class TestClass{
.method public static void test_121_catch(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_121_startTry_1:
  leave lbl_121_exitSEH
lbl_121_endTry_1:
lbl_121_startTry:
  newobj     instance void [mscorlib]System.Exception::.ctor()
   throw
  leave lbl_121_exitSEH
lbl_121_endTry:
lbl_121_startCatch:
   pop
   leave lbl_121_exitSEH
lbl_121_endCatch:
.try  lbl_121_startTry_1 to lbl_121_endTry_1 catch ['mscorlib']System.Exception handler lbl_121_startCatch to lbl_121_endCatch
.try  lbl_121_startTry to lbl_121_endTry catch ['mscorlib']System.Exception handler lbl_121_startCatch to lbl_121_endCatch
lbl_121_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_121_catch()
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
