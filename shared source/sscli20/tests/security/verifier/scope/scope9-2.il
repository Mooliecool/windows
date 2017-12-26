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
//Access a static public field. Should not get VE

.module 'scope9-2.exe'

.assembly 'scope9-2' // as "scope9-2.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.assembly extern another
{
  .hash = (58 CB F9 AA 70 5F AF 80 ED F5 DE 9C 07 C3 B9 A8   // X...p_..........
           12 FF 7E 8D )                                     // ..~.
  .ver 0:0:0:0
}

.class public auto ansi TestClass extends [another]A
{

  .field private class [another]A a
  .method private instance void Bar() il managed
  {
    // Code size       18 (0x12)
    .maxstack  8
   // IL_0000:  ldarg.0
    IL_0001:  ldc.i4.5
    IL_0002:  stsfld      int32 [another]A::iStaticPublic
    IL_0007:  ldsfld     int32 [another]A::iStaticPublic
    IL_000c:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_0011:  ret


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
    IL_0001:  call       instance void [another]A::.ctor()
    IL_0006:  ret
  } // end of method 'TestClass::.ctor'

} // end of class 'TestClass'

//*********** DISASSEMBLY COMPLETE ***********************
