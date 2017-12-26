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
.assembly extern mscorlib
{
  .publickeytoken = (B7 7A 5C 56 19 34 E0 89 )                         
  .ver 1:0:2411:0
}
.assembly callvirt_vt
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.class public value ansi sealed A extends ['mscorlib']System.ValueType
{
	.field public int32[] gcref
         .method virtual void foo(){ret}
}
.class public auto ansi beforefieldinit TestClass
       extends [mscorlib]System.Object
{
  .method public hidebysig static void  Main(string[] args) cil managed
  {
    .entrypoint
    .maxstack  1
    .locals init (value class A V_0)
    ldloca 0
    initobj A
    ldloc 0
    callvirt void A::foo()
    IL_000c:  ret
  } 
  .method public hidebysig specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } 
} 
