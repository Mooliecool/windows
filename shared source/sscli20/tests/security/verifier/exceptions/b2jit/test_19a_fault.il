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
  .ver 0:0:0:0
}
.assembly test_19_fault
{
  .ver 0:0:0:0
}
.module test_19_fault.EXE
.imagebase 0x00400000
.subsystem 0x00000003
.file alignment 512
.corflags 0x00000001
.class private auto ansi TestClass
       extends [mscorlib]System.Object
{
} 
.class private auto ansi TestClass
       extends [mscorlib]System.Object
{
  .method public static void  test_19_fault() cil managed
  {
    .maxstack  8
    .locals init (int32 V_0)
    IL_0000:  ldc.i4.1
    IL_0001:  stloc      V_0
    IL_0005:  ldstr      "Test is running"
    IL_000a:  call       void [mscorlib]System.Console::WriteLine(string)
     IL_0015:  tail.
      IL_0017:  call       void TestClass::foo_19_fault()
                ret
    .try
    {
      IL_000f:  nop
      IL_0010:  leave      IL_001d
    }  
    fault
    {
      IL_001c:  endfinally
    }  
    IL_001d:  ret
  } 
  .method public static void  foo_19_fault() cil managed
  {
    .maxstack  8
    IL_0000:  ldstr      "Test is running"
    IL_0005:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_000a:  ret
  } 
  .method public static void  Main(string[] arg) cil managed
  {
    .entrypoint
    .maxstack  8
    IL_0000:  call       void TestClass::test_19_fault()
    IL_0005:  ret
  } 
  .method public specialname rtspecialname 
          instance void  .ctor() cil managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } 
} 
