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
//In same assembly, access Static AssemblyOrFamily fiel in another class, Should not get VE

.module 'scope9-10.exe'

.assembly 'scope9-10' // as "scope9-10.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.class public auto ansi A extends [mscorlib]System.Object
{

  .field public static int32 iStaticPublic
  .field static family int32 iStaticFamily
  .field static assembly int32 iStaticAssembly
  .field static famorassem int32 iStaticFamAssembly
  .field private static int32 iStaticPrivate

  .field family int32 iFamily
  .field public int32 iPublic
  .field private int32 iPrivate
  .field assembly int32 iAssembly
  .field famorassem int32 iAssFamily
  .field famorassem int32 iAssAndFamily
  .method public instance void Foo() il managed
  {
    // Code size       29 (0x1d)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldc.i4.2
    IL_0002:  stfld      int32 A::iPrivate
    IL_0007:  ldarg.0
    IL_0008:  ldfld      int32 A::iPrivate
    IL_000d:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_0012:  ldsfld     int32 A::iStaticPrivate
    IL_0017:  call       void [mscorlib]System.Console::WriteLine(int32)
    IL_001c:  ret
  } // end of method 'A::Foo'

  .method public specialname rtspecialname instance void .ctor() il managed
  {
    // Code size       14 (0xe)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  ldc.i4.2
    IL_0002:  stfld      int32 A::iPrivate
    IL_0007:  ldarg.0
    IL_0008:  call       instance void [mscorlib]System.Object::.ctor()
    IL_000d:  ret
  } // end of method 'A::.ctor'

} // end of class 'A'

.class public auto ansi TestClass
{
  .field private class A a
  .method private instance void Bar() il managed
  {
    // Code size       17 (0x11)
    .maxstack  8
    IL_0000:  ldc.i4.5
    IL_0001:  stsfld     int32 A::iStaticFamAssembly
    IL_0006:  ldsfld     int32 A::iStaticFamAssembly
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
