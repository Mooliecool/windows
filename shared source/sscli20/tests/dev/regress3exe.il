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

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                
  .hash = (98 31 71 95 B3 A4 51 E8 E2 0D 25 55 57 E7 6E 9E   
           54 8C EF C9 )                                     
  .ver 1:0:3300:0
}
.assembly extern regress3dll
{
  .hash = (11 0E 02 E6 3B 66 92 E7 E3 EC 48 BB 87 AC 91 DB   
           15 21 7E D4 )                                    
  .ver 0:0:0:0
}

.assembly exe
{
  .custom instance void [mscorlib]System.Reflection.AssemblyDelaySignAttribute::.ctor(bool) = ( 01 00 00 )
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module exe.exe
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001

// =============== GLOBAL FIELDS AND METHODS ===================

//Global methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  .method public static void modopt([mscorlib]System.Runtime.CompilerServices.CallConvCdecl) 
        runTest() il managed
  {
    .entrypoint
    .maxstack  1
    .locals init (int32 V_1)
    .try
    {
      .try 
      {
                call       void [regress3dll]C::Doit()
                ldstr      "TEST FAILED"
                call       void ['mscorlib']System.Console::WriteLine(string)
                ldc.i4.1
                stloc      V_1
                leave.s    EndTry
      }  // end try
      catch ['mscorlib']System.Security.VerificationException 
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
  } // end of global method main


