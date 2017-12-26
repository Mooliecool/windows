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
.assembly seh4 {}
.class TestClass{
.method public static void seh4(){
  ldstr      "Test is running"
  call       void [mscorlib]System.Console::WriteLine(class System.String)  
_try_s:
          nop
	  leave lbl_1_exitSEH
_try_e:
_fil_s:
pop
ldc.i4.1
endfilter
_fil_e:
_catch_s:
	  nop
          leave lbl_1_exitSEH
_catch_e:
.try _try_s to _try_e filter 0x100 handler _catch_s to _catch_e
lbl_1_exitSEH:
  ret
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    call void TestClass::seh4()
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
