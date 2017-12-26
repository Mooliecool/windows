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
//access family or Assembly type field in son class in another assembly. Should NOT get VE

.module 'scope8-2.exe'

.assembly 'scope8-2' // as "scope8-2.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.assembly extern another
{
  .hash = (AA 70 58 1F 89 17 AE A0 77 47 61 A5 00 DC DC 2D   // .pX.....wGa....-
           80 3A 7D 88 )                                     // .:}.
  .ver 0:0:0:0
}

.class public auto ansi TestClass extends [another]A
{
  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       25 (0x19)
    .maxstack  2
    .locals init (class TestClass V_0)
    IL_0000:  newobj     instance void TestClass::.ctor()
    IL_0005:  stloc.0
    IL_0006:  ldloc.0
    IL_0007:  ldc.i4.5
    IL_0008:  stfld      int32 [another]A::iAssFamily
    IL_000d:  ldloc.0
    IL_000e:  ldfld      int32 [another]A::iAssFamily
    IL_0013:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_0018:  ret
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
