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
//Indirect load and store box value class. Should not get VE
.module 'excep8-2.exe'
.assembly 'excep8-2' 
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
  .method public static void  Foo(class ['mscorlib']System.Object obj) il managed
  {
    .maxstack  1
    .locals init (value class B V_0)
    IL_0000:  ldarg.0
    IL_0001:  unbox      B
    IL_0006:  ldobj      B
    IL_000b:  stloc.0
    IL_000c:  ldloca.s   V_0
    IL_000e:  ldfld      int32 B::ia
    IL_0013:  call       void ['mscorlib']System.Console::WriteLine(int32)
    IL_0018:  ret
  } 
  .method public static void  Main(class ['mscorlib']System.String[] args) il managed
  {
    .entrypoint
    .maxstack  2
    .locals init (value class B V_0,
             value class A V_1,
             class ['mscorlib']System.Object V_2,
             class ['mscorlib']System.Object V_3)
    IL_0000:  ldloca.s   V_0
    IL_0002:  initobj    B
    IL_0008:  ldloca.s   V_1
    IL_000a:  initobj    A
    IL_0010:  ldloca.s   V_0
    IL_0012:  ldc.i4.s   10
    IL_0014:  stfld      int32 B::ia
    IL_0019:  ldloca.s   V_0
    IL_001b:  ldc.i4.s   9
    IL_001d:  stfld      int32 B::ja
    IL_0022:  ldloca.s   V_1
    IL_0024:  ldc.i4.5
    IL_0025:  stfld      int32 A::ja
    IL_002a:  ldloca.s   V_1
    IL_002c:  ldc.i4.4
    IL_002d:  stfld      int32 A::ia
    IL_0032:  ldnull
    IL_0033:  stloc.2
    IL_0034:  ldnull
    IL_0035:  stloc.3
    .try
    {
      IL_0036:  ldloc.s   V_1
      IL_0038:  box        A
      IL_003d:  stloc.2
      IL_003e:  ldloc.s   V_0
      IL_0040:  box        B
      IL_0045:  stloc.2
      IL_0046:  newobj     instance void ['mscorlib']System.Exception::.ctor()
      IL_004b:  throw
      IL_004c:  br.s       IL_004c
    }  
    catch ['mscorlib']System.Exception 
    {
      IL_004e:  pop
      IL_004f:  ldloca.s   V_3
      IL_0051:  ldloca.s   V_2
      IL_0053:  ldind.ref
      IL_0054:  stind.ref
      IL_0055:  ldloc.3
      IL_0056:  call       void TestClass::Foo(class ['mscorlib']System.Object)
      IL_005b:  leave.s    IL_005d
    }  
    IL_005d:  ret
  } 
  .method public specialname rtspecialname instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
} 
