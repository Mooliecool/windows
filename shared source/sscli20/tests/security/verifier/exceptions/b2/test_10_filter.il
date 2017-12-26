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
.assembly test_10_ter {}
.class TestClass{
.method public static void test_10_filter(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_10_startTry
lbl_10_startTry:
   	newobj     instance void [mscorlib]System.Exception::.ctor()
   	throw
  leave lbl_10_exitSEH
lbl_10_endTry:
lbl_10_startFilter:
  ldc.i4.1
  ldc.i4.1
  endfault
  endfilter
lbl_10_endFilter:
lbl_10_startFilterHandler:
  lbl_10_1:nop
  pop
  leave lbl_10_exitSEH
lbl_10_endFilterHandler:
.try  lbl_10_startTry to lbl_10_endTry filter lbl_10_startFilter handler lbl_10_startFilterHandler to lbl_10_endFilterHandler
lbl_10_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_10_filter()
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
