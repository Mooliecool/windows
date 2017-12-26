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
.assembly AI_newArr{}
.class public auto ansi TestClass extends ['mscorlib']System.Object
{
.method public hidebysig static void Main(class [mscorlib]System.String[]) il managed
  {
    .entrypoint
    .maxstack  1
    .locals init (object V_0)
    IL_0000:  ldc.i4.1
    IL_0001:  newarr     [mscorlib]System.ArgIterator
    IL_0006:  stloc.0
    IL_0007:  ret
  } 
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() cil managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } 
} 
