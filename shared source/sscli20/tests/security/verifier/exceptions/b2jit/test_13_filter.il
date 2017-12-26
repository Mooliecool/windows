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
.assembly test_13_ter {}
.class TestClass{
.method public static void test_13_filter(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_13_startTry
lbl_13_startTry:
  leave lbl_13_exitSEH
lbl_13_endTry:
lbl_13_startFilter:
   newobj     instance void [mscorlib]System.Exception::.ctor()
   throw
  ldc.i4.1
  endfilter
lbl_13_endFilter:
lbl_13_startFilterHandler:
  pop
  leave lbl_13_exitSEH
lbl_13_endFilterHandler:
.try  lbl_13_startTry to lbl_13_endTry filter lbl_13_startFilter handler lbl_13_startFilterHandler to lbl_13_endFilterHandler
lbl_13_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::test_13_filter()
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
