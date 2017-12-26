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

//-desc: access to private nested B defined inside public A

// VTableFixup Directory:
// No data.
.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:0:2411:0
}
.assembly scope21
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 
.module scope21.exe
// MVID: {959F307A-CF8E-4474-ABA2-77B5B2B2DB8F}
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
// Image base: 0x03540000
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
  .method public static void  Foo() il managed
  {
   
    // Code size       18 (0x12)
    .maxstack  1
    .locals init (class A/B V_0)
   
     newobj     instance void A/B::.ctor()
     stloc.0
     ret                 
   }  // end try

  .method public hidebysig static void  Main(string[] args) cil managed
  {
    .entrypoint
    // Code size       7 (0x7)
    .maxstack  1
    .locals init (class A/B V_0, int32 V_1)
    .try {
      .try {
               call       void TestClass::Foo()
               leave.s    EndTry
               ldstr      "TEST FAILED"
               call       void ['mscorlib']System.Console::WriteLine(string)
               ldc.i4.1
               stloc      V_1
               leave.s    EndTry
      }  // end try
      catch ['mscorlib']System.MethodAccessException
      {
               pop
               ldstr      "TEST PASSED"
               call       void ['mscorlib']System.Console::WriteLine(string)
               ldc.i4.0
               stloc      V_1
               leave.s    EndTry

      }  // end handler
      EndTry: leave.s EndIL
    }  // end try 
    catch ['mscorlib']System.Exception 
    {
        pop
        ldstr      "TEST FAILED"
        call       void ['mscorlib']System.Console::WriteLine(string)
        ldc.i4.1
        stloc      V_1
        leave.s    EndIL
    }  // end handler

    EndIL:    ldloc  V_1 
              call       void [mscorlib]System.Environment::set_ExitCode(int32)
              ret
  } // end of method TestClass::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method TestClass::.ctor

} // end of class TestClass

.class public  auto ansi beforefieldinit A
       extends [mscorlib]System.Object
{
  .class auto ansi nested private beforefieldinit B
         extends [mscorlib]System.Object
  {
    .method public hidebysig specialname rtspecialname 
            instance void  .ctor() cil managed
    {
      // Code size       7 (0x7)
      .maxstack  8
      IL_0000:  ldarg.0
      IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
      IL_0006:  ret
    } // end of method B::.ctor

  } // end of class B

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method A::.ctor

} // end of class A

//*********** DISASSEMBLY COMPLETE ***********************
// WARNING: Created Win32 resource file scope21.res
