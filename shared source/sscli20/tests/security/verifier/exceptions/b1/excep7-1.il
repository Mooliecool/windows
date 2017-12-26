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
//In try block, box two value class to one Object inst. In catch block, unbox to one of them. No VE
.module 'excep7-1.exe'
.assembly 'excep7-1' 
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.class value public auto ansi sealed A extends ['mscorlib']System.ValueType
{
  .field public int32 ia
  .field public int32 ja
} 
.class value public auto ansi sealed B extends ['mscorlib']System.ValueType
{
  .field public int32 ia
  .field public int32 ja
} 
.class public auto ansi TestClass extends ['mscorlib']System.Object
{
  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    .maxstack  2
    .locals init (value class B V_0,
             class ['mscorlib']System.Object V_1,
             value class A V_2,
             int32 V_3,
             value class A V_4)
    IL_0000:  ldloca.s   V_0
    IL_0002:  initobj    B
    IL_0008:  ldloca.s   V_0
    IL_000a:  ldc.i4.s   10
    IL_000c:  stfld      int32 B::ia
    IL_0011:  ldloca.s   V_0
    IL_0013:  ldc.i4.s   9
    IL_0015:  stfld      int32 B::ja
    IL_001a:  ldloc.s   V_0
    IL_001c:  box        B
    IL_0021:  stloc.1
    .try
    {
      IL_0022:  ldloca.s   V_2
      IL_0024:  initobj    A
      IL_002a:  ldloca.s   V_2
      IL_002c:  ldc.i4.5
      IL_002d:  stfld      int32 A::ja
      IL_0032:  ldloca.s   V_2
      IL_0034:  ldc.i4.4
      IL_0035:  stfld      int32 A::ia
      IL_003a:  ldloc.s   V_2
      IL_003c:  box        A
      IL_0041:  stloc.1
      IL_0042:  leave.s    IL_0062
    }  
    catch ['mscorlib']System.Exception 
    {
      IL_0044:  pop
      IL_0045:  ldloc.1
      IL_0046:  unbox      A
      IL_004b:  ldobj      A
      IL_0050:  stloc.s    V_4
      IL_0052:  ldloca.s   V_4
      IL_0054:  ldfld      int32 A::ia
      IL_0059:  stloc.3
      IL_005a:  ldloc.3
      IL_005b:  call       void ['mscorlib']System.Console::WriteLine(int32)
      IL_0060:  leave.s    IL_0062
    }  
    IL_0062:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
} 
