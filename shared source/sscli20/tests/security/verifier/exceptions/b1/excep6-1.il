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
.module 'excep6-1.exe'
.assembly 'excep6-1' 
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
             value class B V_4)
    IL_0000:  ldloca.s   V_0
    IL_0002:  initobj    B
    IL_0008:  ldloca.s   V_0
    IL_000a:  ldc.i4.s   10
    IL_000c:  stfld      int32 B::ia
    IL_0011:  ldloca.s   V_0
    IL_0013:  ldc.i4.s   9
    IL_0015:  stfld      int32 B::ja
    IL_001a:  ldnull
    IL_001b:  stloc.1
    .try
    {
      IL_001c:  ldloca.s   V_2
      IL_001e:  initobj    A
      IL_0024:  ldloca.s   V_2
      IL_0026:  ldc.i4.5
      IL_0027:  stfld      int32 A::ja
      IL_002c:  ldloca.s   V_2
      IL_002e:  ldc.i4.4
      IL_002f:  stfld      int32 A::ia
      IL_0034:  ldloc.s   V_2
      IL_0036:  box        A
      IL_003b:  stloc.1
      IL_003c:  ldloc.s   V_0
      IL_003e:  box        B
      IL_0043:  stloc.1
      IL_0044:  newobj     instance void ['mscorlib']System.Exception::.ctor()
      IL_0049:  throw
    }  
    catch ['mscorlib']System.Exception 
    {
      IL_004a:  pop
      IL_004b:  ldloc.1
      IL_004c:  unbox      B
      IL_0051:  ldobj      B
      IL_0056:  stloc.s    V_4
      IL_0058:  ldloca.s   V_4
      IL_005a:  ldfld      int32 B::ia
      IL_005f:  stloc.3
      IL_0060:  ldloc.3
      IL_0061:  call       void ['mscorlib']System.Console::WriteLine(int32)
      IL_0066:  leave.s    IL_0068
    }  
    IL_0068:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
} 
