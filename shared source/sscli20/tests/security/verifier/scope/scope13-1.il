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
//access global public field in another module in same assembly. NO VE 

.module 'scope13-1.exe'

.assembly 'scope13-1' // as "scope13-1.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.file anotherprivatescope.dll
.module extern anotherprivatescope.dll
    //.hash = (C3 B9 9A D5 3A 43 ED 6F A0 2B 5F EE 12 0D 0A 2F   // ....:C.o.+_..../
        //     B7 42 87 AE )                                     // .B..

.class public auto ansi TestClass extends [mscorlib]System.Object
{
  .field private int32 aa
  .method private void Bar() il managed
  {
    // Code size       19 (0x13)
    .maxstack  8
   // IL_0000:  ldarg.0
	ldc.i4.7
	call       void [mscorlib]System.Console::WriteLine(int32)

    IL_0001:  ldc.i4.5
    IL_0002:  stsfld   int32 [.module anotherprivatescope.dll]::iPublic
    //IL_0007:  ldarg.0
    IL_0008:  ldsfld      int32 [.module anotherprivatescope.dll]::iPublic
    IL_000d:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_0012:  ret
  } // end of method 'TestClass::Bar'

  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       13 (0xd)
    .maxstack  1
      .locals init (class TestClass V_0, int32 V_1)
    .try {
      .try {
               ldc.i4.0
	       call       void [mscorlib]System.Console::WriteLine(int32)
               newobj     instance void TestClass::.ctor()
               call       instance void TestClass::Bar()
               leave.s    EndTry
               ldstr      "TEST PASSED"
               call       void ['mscorlib']System.Console::WriteLine(string)
               ldc.i4.0
               stloc      V_1
               leave.s    EndTry
      }  // end try
      catch ['mscorlib']System.MethodAccessException
      {
               pop
               ldstr      "TEST FAILED"
               call       void ['mscorlib']System.Console::WriteLine(string)
               ldc.i4.1
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
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method 'TestClass::.ctor'

} // end of class 'TestClass'

//*********** DISASSEMBLY COMPLETE ***********************
