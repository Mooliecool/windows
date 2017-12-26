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
//nested class can access outter class private and family field and function. NO VE

.module 'scope16-1.exe'

.assembly 'scope16-1' // as "scope16-1.exe"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.class public auto ansi TestClass extends ['mscorlib']System.Object
{
  .class auto ansi nested private TestClass$Nested extends ['mscorlib']System.Object
  {
    .method public hidebysig static void StaticFunctions() il managed
    {
      // Code size       45 (0x2d)
      .maxstack  8
      IL_0000:  call       void TestClass::StaticPriv()
      IL_0005:  call       void TestClass::StaticPro()
      IL_000a:  call       void TestClass::StaticFam()
      IL_000f:  call       void TestClass::StaticPub()
      IL_0014:  ldc.i4.1
      IL_0015:  stsfld     int32 TestClass::StaticPrivI
      IL_001a:  ldc.i4.2
      IL_001b:  stsfld     int32 TestClass::StaticProI
      IL_0020:  ldc.i4.3
      IL_0021:  stsfld     int32 TestClass::StaticFamI
      IL_0026:  ldc.i4.4
      IL_0027:  stsfld     int32 TestClass::StaticPubI
      IL_002c:  ret
    } // end of method 'TestClass$Nested::StaticFunctions'

    .method public hidebysig specialname rtspecialname 
            instance void .ctor() il managed
    {
      // Code size       7 (0x7)
      .maxstack  8
      IL_0000:  ldarg.0
      IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
      IL_0006:  ret
    } // end of method 'TestClass$Nested::.ctor'

  } // end of class 'TestClass$Nested'

  .field private static int32 StaticPrivI
  .field static family int32 StaticProI
  .field static assembly int32 StaticFamI
  .field public static int32 StaticPubI
  .method private hidebysig static void StaticPriv() il managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method 'TestClass::StaticPriv'

  .method family hidebysig static void StaticPro() il managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method 'TestClass::StaticPro'

  .method assembly hidebysig static void StaticFam() il managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method 'TestClass::StaticFam'

  .method public hidebysig static void StaticPub() il managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method 'TestClass::StaticPub'

  .method private hidebysig instance void
          Priv() il managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method 'TestClass::Priv'

  .method family hidebysig instance void Pro() il managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method 'TestClass::Pro'

  .method assembly hidebysig instance void
          Fam() il managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method 'TestClass::Fam'

  .method public hidebysig instance void Pub() il managed
  {
    // Code size       1 (0x1)
    .maxstack  8
    IL_0000:  ret
  } // end of method 'TestClass::Pub'

  .method public hidebysig static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    // Code size       16 (0x10)
    .maxstack  8
    IL_0000:  call       void TestClass/TestClass$Nested::StaticFunctions()
    IL_0005:  ldstr      "finish call static functions."
    IL_000a:  call       void ['mscorlib']System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000f:  ret
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
