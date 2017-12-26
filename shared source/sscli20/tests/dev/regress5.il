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
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:0:3300:0
}
.assembly regress5
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module regress5.exe
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001

//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
} // end of class MainApp


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class private auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
  .method public hidebysig static void  m() cil managed
  {
    .locals init(value class [mscorlib]System.Object o)
     ldloc o
     isinst      class [mscorlib]System.Object
     pop
     ret
  } // end of method MainApp::m

  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint
    // Code size       43 (0x2b)
    .maxstack  1
    .try
    {
      IL_0000:  call       void MainApp::m()
      IL_0005:  leave.s    IL_001a

    }  // end .try
    catch [mscorlib]System.TypeLoadException
    {
      IL_0007:  pop
      IL_0008:  ldstr      "PASSED: TypeLoad exception thrown"
      IL_000d:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0012:  ldc.i4.0
      IL_0013:  call       void [mscorlib]System.Environment::Exit(int32)
      IL_0018:  leave.s    IL_001a

    }  // end handler
    IL_001a:  ldstr      "FAILED: Verification exception not thrown"
    IL_001f:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_0024:  ldc.i4.1
    IL_0025:  call       void [mscorlib]System.Environment::Exit(int32)
    IL_002a:  ret
  } // end of method MainApp::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    // Code size       7 (0x7)
    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method MainApp::.ctor

} // end of class MainApp

