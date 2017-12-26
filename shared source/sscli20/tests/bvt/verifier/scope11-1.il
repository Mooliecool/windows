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
.module 'scope11-1.exe'
.assembly 'scope11-1' 
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.class public auto ansi A extends [mscorlib]System.Object
{
  .field family int32 i
  .method family instance void foo() il managed
  {
    .maxstack  8
    IL_0000:  ldstr      "This is foo function in A class."
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } 
} 
.class public auto ansi B extends A
{
  .field public int32 j
  .method public instance void bar() il managed
  {
    .maxstack  8
    IL_0000:  ldstr      "This is bar function in B class."
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(class ['mscorlib']System.String)
    IL_000a:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void A::.ctor()
    IL_0006:  ret
  } 
} 
.class public auto ansi TestClass extends A
{
  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    .maxstack  1
    .locals init (class B V_0)
    IL_0000:  newobj     instance void B::.ctor()
    IL_0005:  stloc.0
    IL_0006:  ldloc.0
    IL_0007:  call       instance void A::foo()
    IL_000c:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void A::.ctor()
    IL_0006:  ret
  } 
} 
