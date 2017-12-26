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

.module 'anothermethodnoassm.dll'

.assembly 'anothermethodnoassm' // as "anothermethodnoassm.dll"
 { 
.custom instance void [mscorlib]System.Security.AllowPartiallyTrustedCallersAttribute::.ctor() = ( 01 00 00 00 )
 
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
 

.class public auto ansi A extends [mscorlib]System.Object
{
  .method public static void StaticPublicFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is StaticPublicFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::StaticPublicFoo'

  .method family static void StaticFamilyFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is StaticFamilyFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::StaticFamilyFoo'

  .method private static void StaticPrivateFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is StaticPrivateFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::StaticPrivateFoo'

  .method assembly static void StaticAssemblyFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is StaticFamilyFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::StaticAssemblyFoo'

  .method famorassem static void StaticFamOrAssmFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is StaticFamOrAssemFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::StaticFamOrAssmFoo'

  .method famorassem static void StaticFamAndAssmFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is StaticFamAndAssemFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::StaticFamAndAssmFoo'

  .method public instance void PublicFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is PublicFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::PublicFoo'

  .method family instance void FamilyFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is FamilyFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::FamilyFoo'

  .method private instance void PrivateFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is PrivateFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::PrivateFoo'

  .method assembly instance void AssemblyFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is FamilyFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::AssemblyFoo'

  .method famorassem instance void FamOrAssmFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is FamOrAssemFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::FamOrAssmFoo'

  .method famorassem instance void FamAndAssmFoo() il managed
  {
    // Code size       11 (0xb)
    .maxstack  8
    IL_0000:  ldstr      "This is FamAndAssemFoo in class A"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } // end of method 'A::FamAndAssmFoo'

  .method public specialname rtspecialname instance void .ctor() il managed
  {
    // Code size       7 (0x7)
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } // end of method 'A::.ctor'

} // end of class 'A'

//*********** DISASSEMBLY COMPLETE ***********************
