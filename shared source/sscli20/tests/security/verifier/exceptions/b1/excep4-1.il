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
//in try bloc, instance could be the smaller class. in catch block, try to use uncommon field. should get VE 
.module 'excep4-1.exe'
.assembly 'excep4-1' 
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.class public auto ansi C extends ['mscorlib']System.Object
{
  .field public int32 ic
  .field public int32 jc
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
} 
.class public auto ansi A extends C
{
  .field public int32 ia
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void C::.ctor()
    IL_0006:  ret
  } 
} 
.class public auto ansi B extends C
{
  .field public int32 ib
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void C::.ctor()
    IL_0006:  ret
  } 
} 
.class public auto ansi TestClass extends ['mscorlib']System.Object
{
  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    .maxstack  1
    .locals init (class A V_0,
             int32 V_1)
    IL_0000:  newobj     instance void A::.ctor()
    IL_0005:  stloc.0
    .try
    {
      IL_0006:  newobj     instance void A::.ctor()
      IL_000b:  stloc.0
      IL_000c:  newobj     instance void C::.ctor()
      IL_0016:  stloc.0
      IL_0017:  newobj     instance void A::.ctor()
      IL_001c:  stloc.0
      IL_001d:  leave.s    IL_002f
    }  
    catch ['mscorlib']System.Exception 
    {
      IL_001f:  pop
      IL_0020:  ldloc.0
      IL_0021:  ldfld      int32 A::ia
      IL_0026:  stloc.1
      IL_0027:  ldloc.1
      IL_0028:  call       void ['mscorlib']System.Console::WriteLine(int32)
      IL_002d:  leave.s    IL_002f
    }  
    IL_002f:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
} 
