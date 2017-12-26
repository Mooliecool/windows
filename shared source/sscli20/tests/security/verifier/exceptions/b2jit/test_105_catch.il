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
.assembly test_105_catch {}
.class TestClass{
.method public static void test_105_catch(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
lbl_105_UP:
lbl_105_startTry:
  newobj     instance void [mscorlib]System.Exception::.ctor()
   throw
  leave lbl_105_exitSEH
lbl_105_endTry:
lbl_105_startCatch:
  lbl_105_1: nop
  ldloc locInt1
   brtrue -1000
  leave lbl_105_exitSEH
lbl_105_endCatch:
.try  lbl_105_startTry to lbl_105_endTry catch ['mscorlib']System.Exception handler lbl_105_startCatch to lbl_105_endCatch
lbl_105_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_105_catch()
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
