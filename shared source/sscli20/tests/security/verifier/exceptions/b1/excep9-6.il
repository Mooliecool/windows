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
//filter block fall thru. Has VE
.module 'excep9-6.exe'
.assembly 'excep9-6' 
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
    .maxstack  2
    .locals init (class C V_0,
             int32 V_1)
    IL_0000:  newobj     instance void C::.ctor()
    IL_0005:  stloc.0
	br IL_0006
    .try
    {
      IL_0006:  newobj     instance void A::.ctor()
      IL_000b:  stloc.0
      IL_000c:  ldloc.0
      IL_000d:  castclass  A
      IL_0012:  ldc.i4.5
      IL_0013:  stfld      int32 C::ic
      IL_0014:  leave.s    IL_0033
    }  
    filter 
    {
      ldc.i4.5
      endfilter
    } 
    {
	pop
      IL_001f:  ldloc.0
      IL_0025:  ldfld      int32 C::ic
      IL_002a:  stloc.1
      IL_002b:  ldloc.1
      IL_002c:  call       void ['mscorlib']System.Console::WriteLine(int32)
    }  
    IL_0033:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
} 
