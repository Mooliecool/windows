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
//ldflda try to access the invisible field, should get VE

.module 'scope5-1.exe'

.assembly 'scope5-1' // as "scope5-1.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.class public auto ansi A extends [mscorlib]System.Object
{
  .field family int32 i
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method 'A::.ctor'

} // end of class 'A'

.class public auto ansi B extends A
{
  .field public int32 j
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void A::.ctor()
    IL_0006:  ret
  } // end of method 'B::.ctor'

} // end of class 'B'

.class public auto ansi TestClass extends A
{
  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       41 (0x29)
    .maxstack  2
    .locals init (class A V_0,
             class B V_1,
             class TestClass V_2,
             int32 V_3)
    IL_0000:  newobj     instance void B::.ctor()
    IL_0005:  stloc.1
    IL_0006:  ldloc.1
    IL_0007:  stloc.0

		ldloc.0
		ldflda int32 A::i
		ldind.i
   
    IL_0023:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_0028:  ret
  } // end of method 'TestClass::Main'

  .method public specialname rtspecialname instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void A::.ctor()
    IL_0006:  ret
  } // end of method 'TestClass::.ctor'

} // end of class 'TestClass'

//*********** DISASSEMBLY COMPLETE ***********************
