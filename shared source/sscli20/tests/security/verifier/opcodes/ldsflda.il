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
.assembly extern mscorlib { }
.assembly 'ldsflda' { }
.class public auto ansi beforefieldinit Test
       extends [mscorlib]System.Object
{
  .field public int64 m_fld1
  .field private static string m_sfld2
  .method public hidebysig instance void Negative1() cil managed
  {
	.maxstack  8
	.emitbyte 0x7F		
	.emitbyte 0xff
	.emitbyte 0xff
	.emitbyte 0xff
	.emitbyte 0x04
	call       void [mscorlib]System.Console::WriteLine(int64)
	ret
  }
  .method public hidebysig instance void Negative2() cil managed
  {
	.maxstack  8
	.emitbyte 0x7F		
	.emitbyte 0x11
	.emitbyte 0x00
	.emitbyte 0x00
	.emitbyte 0x00
	call       void [mscorlib]System.Console::WriteLine(int64)
	ret
  }
  .method public hidebysig instance void Negative3() cil managed
  {
	.maxstack  8
	.emitbyte 0x7F		
	.emitbyte 0x01
	.emitbyte 0x00
	.emitbyte 0x00
	.emitbyte 0xff
	call       void [mscorlib]System.Console::WriteLine(int64)
	ret
  }
  .method public hidebysig instance void Negative4() cil managed
  {
	.maxstack  8
	.emitbyte 0x7F		
	.emitbyte 0xFF
	.emitbyte 0xFF
	.emitbyte 0xFF
	.emitbyte 0xFF
	call       void [mscorlib]System.Console::WriteLine(int64)
	ret
  }
  .method public hidebysig static int32 Main() cil managed
  {
    .entrypoint
    .maxstack  1
    .locals init (class Test V_0, int32 V_1)
	newobj     instance void Test::.ctor()
	stloc.0
    .try
    {
			ldloc.0
			callvirt   instance void Test::Negative1()
			leave.s    IL_FAIL
    }  
    catch [mscorlib]System.Security.VerificationException 
    {
			pop
			leave.s    IL_NEXT_1
    }  
IL_NEXT_1:
    .try
    {
			ldloc.0
			callvirt   instance void Test::Negative2()
			leave.s    IL_FAIL
    }  
    catch [mscorlib]System.Security.VerificationException 
    {
			pop
			leave.s    IL_NEXT_2
    }  
IL_NEXT_2:
    .try
    {
			ldloc.0
			callvirt   instance void Test::Negative3()
			leave.s    IL_FAIL
    }  
    catch [mscorlib]System.Security.VerificationException 
    {
			pop
			leave.s    IL_NEXT_3
    }  
IL_NEXT_3:
    .try
    {
			ldloc.0
			callvirt   instance void Test::Negative4()
			leave.s    IL_FAIL
    }  
    catch [mscorlib]System.Security.VerificationException 
    {
			pop
			leave.s    IL_NEXT_4
    }  
IL_NEXT_4:
    IL_0011:  ldstr      "PASSED"
    IL_0016:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_001b:  ldc.i4.s   0
    IL_001d:  stloc.1
    IL_001e:  br.s       IL_002f
    IL_FAIL:  ldstr      "FAILED"
    IL_0025:  call       void [mscorlib]System.Console::WriteLine(string)
    IL_002a:  ldc.i4.s   1
    IL_002c:  stloc.1
    IL_002d:  br.s       IL_002f
    IL_002f:  ldloc.1
    IL_0030:  ret
  } 
  .method public hidebysig specialname rtspecialname 
          instance void .ctor() cil managed
  {
    .maxstack  8
    IL_0000:  ldarg.0
    IL_0001:  call       instance void [mscorlib]System.Object::.ctor()
    IL_0006:  ret
  } 
} 
