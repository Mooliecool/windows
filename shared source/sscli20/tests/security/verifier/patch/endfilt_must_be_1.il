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
.assembly endfilt_must_be_1 {}
.class TestClass{
.method public static void endfilt_must_be_1(){
   ldstr      "Test is running"
   call       void [mscorlib]System.Console::WriteLine(class System.String)  
IL_TRY:
   leave.s IL_END
IL_FILTER:
   ldc.i4.0
   endfilter
IL_EXCEPT:
   leave.s IL_END
IL_END:
   ret
.try IL_TRY to IL_FILTER filter IL_FILTER handler IL_EXCEPT to IL_END
}
.method public static void Main(class ['mscorlib']System.String[] arg)
{
    .entrypoint
    call void TestClass::endfilt_must_be_1()
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
