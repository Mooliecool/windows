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

.assembly extern mscorlib {}
//access family or Assembly type field in another assembly. No inhei relationship. Should get VE

.module 'scope8-3.exe'

.assembly 'scope8-3' // as "scope8-3.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.assembly extern another
{
  .hash = (AA 70 58 1F 89 17 AE A0 77 47 61 A5 00 DC DC 2D   // .pX.....wGa....-
           80 3A 7D 88 )                                     // .:}.
  .ver 0:0:0:0
}

.class public auto ansi TestClass
{
  .method public static void  Foo() il managed
  {
   
    // Code size       18 (0x12)
    .maxstack  10
    .locals init (class [another]A V_0)
               newobj     instance void [another]A::.ctor()
               stloc.0
               ldloc.0
               ldc.i4.5
               stfld      int32 [another]A::iAssFamily
               ldloc.0
               ldfld      int32 [another]A::iAssFamily
               call       void [mscorlib]System.Console::WriteLine(int32)        
     ret                 
   }  
  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       25 (0x19)
    .maxstack  2
    .locals init (int32 V_1)
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
      catch ['mscorlib']System.FieldAccessException
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
  } // end of method 'TestClass::Main'

  .method public specialname rtspecialname instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } // end of method 'TestClass::.ctor'

} // end of class 'TestClass'

//*********** DISASSEMBLY COMPLETE ***********************
