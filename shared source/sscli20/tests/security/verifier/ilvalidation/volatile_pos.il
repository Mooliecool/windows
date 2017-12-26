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
.assembly volatile_pos
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module volatile_pos.exe
.class value public auto ansi volatile_pos extends [mscorlib]System.Object
{
  .field private static class System.String f_str
  .field private class System.String f_str2
  .method public hidebysig static void Test() il managed
  {
    .maxstack  64
   .locals init (int32 V_0,
             class volatile_pos V_1,
		unsigned int8[] V_2)
    ldc.i4.0
    stloc.0
    ldstr      "In the Test"
    call       void [mscorlib]System.Console::WriteLine(class System.String)
    volatile.
    ldsfld     class System.String [mscorlib]System.String::Empty
    call       void [mscorlib]System.Console::WriteLine(class System.String)
    ldstr      "This is a static field"
    volatile.
    stsfld     class System.String volatile_pos::f_str
    volatile.
    ldsfld     class System.String volatile_pos::f_str
    call       void [mscorlib]System.Console::WriteLine(class System.String)
    ldloca     0
    ldloca     0
    volatile.
    ldind.i4
    volatile.
    stind.i4
    newobj     instance void volatile_pos::.ctor()
    stloc.1
    ldloc.1
    ldstr      "This is a non-static field"
    volatile.
    stfld      class System.String volatile_pos::f_str2
    ldloc.1
    volatile.
    ldfld     class System.String volatile_pos::f_str2
    call       void [mscorlib]System.Console::WriteLine(class System.String)
    ldloca 0
    ldloca 0
    volatile.
    ldobj	valuetype [mscorlib]System.Int32
    volatile.
    stobj	valuetype [mscorlib]System.Int32
    ret
  } 
  .method public hidebysig static void Main() il managed
  {
    .entrypoint
    .maxstack  2
    .locals init (class [mscorlib]System.InvalidProgramException V_0,
             class [mscorlib]System.Exception V_1)
    .try
    {
      IL_0000:  call       void volatile_pos::Test()
      IL_0005:  ldstr      "CORRECT. No exceptions."
      IL_000a:  call       void [mscorlib]System.Console::WriteLine(class System.String)
      IL_000f:  ldc.i4.s   0
      IL_0011:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
      IL_0016:  leave.s    IL_0054
    }  
    catch [mscorlib]System.InvalidProgramException 
    {
      IL_0018:  stloc.0
      IL_0019:  ldstr      "WRONG. Verifier exception:\n"
      IL_001e:  ldloc.0
      IL_001f:  callvirt   instance class System.String [mscorlib]System.Exception::ToString()
      IL_0024:  call       class System.String [mscorlib]System.String::Concat(class System.String,
                                                                               class System.String)
      IL_0029:  call       void [mscorlib]System.Console::WriteLine(class System.String)
      IL_002e:  ldc.i4.1
      IL_002f:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
      IL_0034:  leave.s    IL_0054
    }  
    catch [mscorlib]System.Exception 
    {
      IL_0036:  stloc.1
      IL_0037:  ldstr      "WRONG. CHECK TESTING CONDITIONS. Exception:\n"
      IL_003c:  ldloc.1
      IL_003d:  callvirt   instance class System.String [mscorlib]System.Exception::ToString()
      IL_0042:  call       class System.String [mscorlib]System.String::Concat(class System.String,
                                                                               class System.String)
      IL_0047:  call       void [mscorlib]System.Console::WriteLine(class System.String)
      IL_004c:  ldc.i4.1
      IL_004d:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
      IL_0052:  leave.s    IL_0054
    }  
    IL_0054:  ret
  } 
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() il managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } 
} 
