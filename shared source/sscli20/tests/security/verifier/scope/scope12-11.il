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
//in same assembly and but not son class call Assembly or Family founction, should not get VE

.module 'scope12-11.exe'

.assembly 'scope12-11' // as "scope12-11.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.module extern 'anothermethodnoassem.dll'
.file anothermethodnoassem.dll
    .hash = (79 0D 9F 86 6F A5 BC 15 E2 94 3F 25 46 23 FC 4F   // y...o.....?%F#.O
             D4 FA 50 7E )                                     // ..P~

.class public auto ansi TestClass
{
  .method private instance void Bar() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  newobj     instance void [.module anothermethodnoassem.dll]A::.ctor()
    IL_0001:  call       instance void [.module anothermethodnoassem.dll]A::FamOrAssmFoo()
    IL_0006:  ret
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
