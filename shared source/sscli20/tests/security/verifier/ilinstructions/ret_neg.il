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
.assembly ldloca_neg
{
  .hash algorithm 0x00008004
  .ver 0:0:0:0
}
.module localloc_neg.exe
.class public auto ansi localloc_negt extends [mscorlib]System.Object
{
  .method public hidebysig static string Test() il managed
  {
    .maxstack  8
    .locals init (int32 V_0,
             int32 V_1)
    ldstr      "In the Test"
    call       void [mscorlib]System.Console::WriteLine(class [mscorlib]System.String)
    ldc.i4	10
    ret
  }
  .method public hidebysig static void Main() il managed
  {
    .entrypoint
    .maxstack  2
    .locals init (class [mscorlib]System.Security.VerificationException V_0,
             class [mscorlib]System.Exception V_1)
    .try
    {
      IL_0000:  call       string localloc_negt::Test()
      IL_0005:  ldstr      "WRONG. No exceptions. Make sure your policy is set"
      + " to Everything"
      IL_000a:  call       void [mscorlib]System.Console::WriteLine(class System.String)
      IL_000f:  ldc.i4.1
      IL_0010:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
      IL_0015:  leave.s    IL_0053
    }  
    catch [mscorlib]System.Security.VerificationException 
    {
      IL_0017:  stloc.0
      IL_0018:  ldstr      "CORRECT. Verifier exception:\n"
      IL_001d:  ldloc.0
      IL_001e:  callvirt   instance class System.String [mscorlib]System.Exception::ToString()
      IL_0023:  call       class System.String [mscorlib]System.String::Concat(class System.String,
                                                                               class System.String)
      IL_0028:  call       void [mscorlib]System.Console::WriteLine(class System.String)
      IL_002d:  ldc.i4   0
      IL_002e:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
      IL_0033:  leave.s    IL_0053
    }  
    catch [mscorlib]System.Exception 
    {
      IL_0035:  stloc.1
      IL_0036:  ldstr      "WRONG. Exception:\n"
      IL_003b:  ldloc.1
      IL_003c:  callvirt   instance class System.String [mscorlib]System.Exception::ToString()
      IL_0041:  call       class System.String [mscorlib]System.String::Concat(class System.String,
                                                                               class System.String)
      IL_0046:  call       void [mscorlib]System.Console::WriteLine(class System.String)
      IL_004b:  ldc.i4.1
      IL_004c:  call       void [mscorlib]System.Environment::set_ExitCode(int32)
      IL_0051:  leave.s    IL_0053
    }  
    IL_0053:  ret
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
