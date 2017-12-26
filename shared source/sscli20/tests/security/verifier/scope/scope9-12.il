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

// VTableFixup Directory:
// No data.
.subsystem 0x00000003
.corflags 0x00000001
.assembly extern mscorlib
{
  .publickeytoken = (03 68 91 16 D3 A4 AE 33 )                         // .h.....3
  .hash = (3F 81 A5 79 23 C8 51 3F 40 D2 43 39 38 05 79 35   // ?..y#.Q?@.C98.y5
           5A 43 D8 77 )                                     // ZC.w
  .ver 1:0:2101:0
}
.assembly 'scope9-12' // as "scope9-12"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  // --- The following custom attribute is added automatically, do not uncomment -------
  //  .custom instance void ['mscorlib']System.Diagnostics.DebuggableAttribute::.ctor(bool,
  //                                                                                  bool) = ( 01 00 00 01 00 00 ) 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 
.module 'scope9-12.exe'
// MVID: {A6621F07-5296-479C-9971-180DBE86B0CA}
.class public auto ansi A
       extends ['mscorlib']System.Object
{
  .field public static class System.Object o
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } // end of method 'A::.ctor'

} // end of class 'A'

.class public auto ansi TestClass
       extends ['mscorlib']System.Object
{
  .method public hidebysig static void Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       51 (0x33)
    .maxstack  8
    IL_0000:  ldnull
    IL_0001:  stsfld     class System.Object A::o
    IL_0006:  ldsfld     class System.Object A::o
    IL_000b:  brfalse.s  IL_0014

    IL_000d:  ldsfld     class System.Object A::o
    IL_0012:  br.s       IL_0019

    IL_0014:  ldstr      "null"
    IL_0019:  call       void ['mscorlib']System.Console::WriteLine(class System.Object)
    IL_001e:  newobj     instance void ['mscorlib']System.Object::.ctor()
    IL_0023:  stsfld     class System.Object A::o
    IL_0028:  ldsfld     class System.Object A::o
    IL_002d:  call       void ['mscorlib']System.Console::WriteLine(class System.Object)
    IL_0032:  ret
  } // end of method 'TestClass::Main'

  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } // end of method 'TestClass::.ctor'

} // end of class 'TestClass'

//*********** DISASSEMBLY COMPLETE ***********************
