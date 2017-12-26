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
.assembly extern rvafield_dll
{
  .ver 0:0:0:0
}
.assembly rvafield_exe
{

  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module rvafield_exe.exe
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001

//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class public auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
} // end of class MainApp


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class public auto ansi beforefieldinit MainApp
       extends [mscorlib]System.Object
{
  .method public hidebysig static void  Bar() cil managed
  {

    .maxstack  0
    IL_0000:  ret
  } // end of method MainApp::Bar

  .method public hidebysig static void  Main() cil managed
  {
    .entrypoint

    .maxstack  0
    IL_0000:  call       void MainApp::Bar()
    IL_0005:  call       void [rvafield_dll]MainDll::Foo()
    IL_000a:  ret
  } // end of method MainApp::Main

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {

    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method MainApp::.ctor

} // end of class MainApp


// =============================================================

