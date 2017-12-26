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
.assembly test_13_catch
{
  .ver 0:0:0:0
}
.module test_13_catch.EXE
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
  .method public static void  test_13_catch() cil managed
  {
    .maxstack  8
    .locals init (int32 V_0)
    IL_0000:  ldc.i4.1
    IL_0001:  stloc      V_0
    IL_0005:  ldstr      "Test is running"
    IL_000a:  call       void [mscorlib]System.Console::WriteLine(string)
    .try
    {
      IL_000f:  ldstr      "ExpMsg"
      IL_0014:  newobj     instance void [mscorlib]System.ArgumentException::.ctor(string)
      IL_0019:  throw
      IL_001a:  leave      IL_0058
    }  
    catch [mscorlib]System.ArgumentException 
    {
      IL_001f:  ldstr      "inside catch"
      IL_0024:  call       void [mscorlib]System.Console::WriteLine(string)
      IL_0029:  call       void [mscorlib]System.Console::WriteLine(object)
      .try
      {
        IL_002e:  ldstr      "inside try1"
        IL_0033:  call       void [mscorlib]System.Console::WriteLine(string)
        .try {
         IL_0038:  rethrow
        }
        finally {
         endfinally
        }
        IL_003a:  leave      IL_0053
      }  
      catch [mscorlib]System.ArgumentException 
      {
        IL_003f:  ldstr      "inside catch1"
        IL_0044:  call       void [mscorlib]System.Console::WriteLine(string)
        IL_0049:  call       void [mscorlib]System.Console::WriteLine(object)
        IL_004e:  leave      IL_0053
      }  
      IL_0053:  leave      IL_0058
    }  
    IL_0058:  ret
  } 
  .method public static void  Main(string[] arg) cil managed
  {
    .entrypoint
    .maxstack  8
    IL_0000:  call       void TestClass::test_13_catch()
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
