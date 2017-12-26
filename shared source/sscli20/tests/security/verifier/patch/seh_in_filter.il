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
.assembly seh_in_filer {}
.class TestClass{
.method public static void seh_in_filer(){
.locals init(int32 locInt1)
  ldc.i4.1
  stloc locInt1
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
br lbl_1_startTry
lbl_1_startTry:
     lbl_1_1:nop
  leave lbl_1_exitSEH
lbl_1_endTry:
lbl_1_startFilter:
  .try{leave lbl_1_exitSEH}catch [mscorlib]System.ArgumentException{leave lbl_1_exitSEH}
  pop 
  ldc.i4.1
  endfilter
lbl_1_endFilter:
lbl_1_startFilterHandler:
  leave lbl_1_exitSEH
lbl_1_endFilterHandler:
.try  lbl_1_startTry to lbl_1_endTry filter lbl_1_startFilter handler lbl_1_startFilterHandler to lbl_1_endFilterHandler
lbl_1_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::seh_in_filer()
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
