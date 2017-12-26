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
//Three level inher, in catch block, only access common field. No VE
.module 'excep5-1.exe'
.assembly 'excep5-1' 
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
.class public auto ansi B extends A
{
  .field public int32 ib
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void A::.ctor()
    IL_0006:  ret
  } 
} 
.class public auto ansi TestClass extends ['mscorlib']System.Object
{
  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    .maxstack  2
    .locals init (class B V_0,
             int32 V_1)
    IL_0000:  newobj     instance void B::.ctor()
    IL_0005:  stloc.0
    .try
    {
      IL_0011:  newobj     instance void B::.ctor()
      IL_0016:  stloc.0
      IL_0017:  ldloc.0
      IL_0018:  ldc.i4.5
      IL_0019:  stfld      int32 A::ia
      IL_001e:  ldloc.0
      IL_001f:  ldc.i4.4
      IL_0020:  stfld      int32 C::jc
      IL_0025:  newobj     instance void ['mscorlib']System.Exception::.ctor()
      IL_002a:  throw
    }  
    catch ['mscorlib']System.Exception 
    {
      IL_002b:  pop
      IL_002c:  ldloc.0
      IL_002d:  ldfld      int32 A::ia
      IL_0032:  stloc.1
      IL_0033:  ldloc.1
      IL_0034:  call       void ['mscorlib']System.Console::WriteLine(int32)
      IL_0039:  ldloc.0
      IL_003a:  ldfld      int32 C::jc
      IL_003f:  stloc.1
      IL_0040:  ldloc.1
      IL_0041:  call       void ['mscorlib']System.Console::WriteLine(int32)
      IL_0046:  leave.s    IL_0048
    }  
    IL_0048:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
} 
