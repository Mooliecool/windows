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
//In one assembly, No inheir relationship, access family And assembly field. Should get VE

.module 'scope8-5.exe'

.assembly 'scope8-5' // as "scope8-5.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.class public auto ansi A extends [mscorlib]System.Object
{
  .field family int32 iFamily
  .field public int32 iPublic
  .field private int32 iPrivate
  .field assembly int32 iAssembly
  .field famorassem int32 iAssFamily
  .field famandassem int32 iAssAndFamily
  .method public instance void Foo() il managed
  {
    // Code size       19 (0x13)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldc.i4.2
    IL_0002:  stfld      int32 A::iPrivate
    IL_0007:  ldarg.0
    IL_0008:  ldfld      int32 A::iPrivate
    IL_000d:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_0012:  ret
  } // end of method 'A::Foo'

  .method public specialname rtspecialname instance void .ctor() il managed
  {
    // Code size       14 (0xe)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldc.i4.2
    IL_0002:  stfld      int32 A::iPrivate
    IL_0007:  ldarg.0
    IL_0008:  call       instance void [mscorlib]System.Object::.ctor()
    IL_000d:  ret
  } // end of method 'A::.ctor'

} // end of class 'A'

.class public auto ansi TestClass
{
  .method public static void Foo() il managed
  {
    // Code size       25 (0x19)
    .maxstack  2
    .locals init (class A V_0)
    IL_0000:  newobj     instance void A::.ctor()
    IL_0005:  stloc.0
    IL_0006:  ldloc.0
    IL_0007:  ldc.i4.5
    IL_0008:  stfld      int32 A::iAssAndFamily
    IL_000d:  ldloc.0
    IL_000e:  ldfld      int32 A::iAssAndFamily
    IL_0013:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_0018:  ret
  } // end of method 'TestClass::Foo'

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
