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
//In same assembly but in two il files, access assembly file in another class, should not get VE

.module 'scope10-1.exe'

.assembly 'scope10-1' // as "scope10-1.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.module extern 'anothernoassm.dll'
.file anothernoassm.dll
    .hash = (E2 D1 5F FD 52 92 6A A4 D7 C8 2E 4D 94 B5 19 7E   // .._.R.j....M...~
             1D 33 87 FD )                                     // .3..

.class public auto ansi TestClass
{

  .field private class [.module anothernoassm.dll]A a
  .method private instance void Bar() il managed
  {
    // Code size       17 (0x11)
    .maxstack  8
    IL_0000:  ldc.i4.5
    IL_0001:  stsfld     int32 [.module anothernoassm.dll]A::iStaticAssembly
    IL_0006:  ldsfld     int32 [.module anothernoassm.dll]A::iStaticAssembly
    IL_000b:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_0010:  ret
  } // end of method 'TestClass::Bar'

  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       13 (0xd)
    .maxstack  1
    .locals init (class TestClass V_0)
    IL_0000:  newobj     instance void TestClass::.ctor()
    IL_0005:  stloc.0
    IL_0006:  ldloc.0
    IL_0007:  call       instance void TestClass::Bar()
    IL_000c:  ret
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
