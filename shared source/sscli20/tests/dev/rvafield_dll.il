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

// Variation on RVA read/write test

.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         // .z\V.4..
  .ver 1:0:3300:0
}
.assembly rvafield_dll
{

  //  .custom instance void [mscorlib]System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module rvafield_dll.dll
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001

.field static public int32 iPublic at MyLabel1
.data MyLabel1 = int32(12)


//
// ============== CLASS STRUCTURE DECLARATION ==================
//
.class public auto ansi beforefieldinit MainDll
       extends [mscorlib]System.Object
{
} // end of class MainDll


// =============================================================


// =============== GLOBAL FIELDS AND METHODS ===================


// =============================================================


// =============== CLASS MEMBERS DECLARATION ===================
//   note that class flags, 'extends' and 'implements' clauses
//          are provided here for information only

.class public auto ansi beforefieldinit MainDll
       extends [mscorlib]System.Object
{
  .method public hidebysig static void  Foo() cil managed
  {

    .maxstack  1
    IL_0000:  ldc.i4.5
    IL_0002:  stsfld   int32 iPublic

    IL_0008:  ldsfld      int32 iPublic
    IL_0001:  call       void [mscorlib]System.Console::WriteLine(int32)

    IL_0006:  ret
  } // end of method MainDll::Foo

  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {

    .maxstack  1
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method MainDll::.ctor

} // end of class MainDll


// =============================================================


