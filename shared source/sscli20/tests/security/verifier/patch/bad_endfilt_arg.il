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
.assembly bad_endfilt_arg {}
.class TestClass{
.method public static void bad_endfilt_arg(){
   ldstr      "Test is running"
   call       void [mscorlib]System.Console::WriteLine(class System.String) 
IL_TRY:
   newobj void [mscorlib]System.Object::.ctor() 
   throw
   leave.s IL_END
IL_FILTER:
   ldstr      "Test is in filter"
   call       void [mscorlib]System.Console::WriteLine(class System.String) 
   pop
   ldc.r4 1
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
    call void TestClass::bad_endfilt_arg()
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
