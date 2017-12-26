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
.subsystem 0x00000003
.corflags 0x00000001
.assembly extern mscorlib
{
  .publickeytoken = (03 68 91 16 D3 A4 AE 33 )                         
  .ver 1:0:2113:0
}
.assembly 'excep11-4' 
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module 'excep11-4.exe'
.class public auto ansi TestClass
       extends ['mscorlib']System.Object
{
  .method public hidebysig static void  Main(class System.String[] args) il managed
  {
    .entrypoint
    .maxstack  8
    IL_0000:  call       void TestClass::bar()
    IL_0005:  ret
  } 
  .method public hidebysig static void bar() il managed
  {
try_0:
  leave end_0
catch_0:
  pop
  try_1:
    leave end_1
  fault_1:
    try_2:
      leave end_2
    catch_2:
      pop
      leave end_1
    end_2:
  endfinally
  end_1:
  leave end_0
end_0:
  ret
.try try_2 to catch_2 catch ['mscorlib']System.Exception handler catch_2 to end_2
.try try_1 to fault_1 fault handler fault_1 to end_1
.try try_0 to catch_0 catch ['mscorlib']System.Exception handler catch_0 to end_0
  } 
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void ['mscorlib']System.Object::.ctor()
    IL_0006:  ret
  } 
} 
