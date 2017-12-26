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
.assembly 'sizeof' { }

.class public auto ansi beforefieldinit Test extends [mscorlib]System.Object {

.method public hidebysig instance void Positive1() cil managed
{
	.maxstack  8
	sizeof object
	pop
	ret
}
.method public hidebysig instance void Negative2() cil managed
{
	.maxstack  8
	.emitbyte 0xFE
	.emitbyte 0x1C
	.emitbyte 0xff
	.emitbyte 0x00
	.emitbyte 0x00
	.emitbyte 0xff
	pop
	ret
}
.method public hidebysig instance void Positive3() cil managed
{
	.maxstack  8
	sizeof Test
	pop
	ret
}
.method public hidebysig static int32 Main() cil managed
{
	.entrypoint
	.maxstack 8
	.locals init (class Test V_0)
	newobj     instance void Test::.ctor()
	stloc.0

	ldloc.0
	callvirt instance void Test::Positive1()
	
IL_NEXT_1:
	.try {
		ldloc.0
		callvirt instance void Test::Negative2()
		ldstr "Negative2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_2
	}
IL_NEXT_2:
	ldloc.0
	callvirt instance void Test::Positive3()

IL_NEXT_3:
	ldc.i4 0
	ldstr "PASSED"
	br.s IL_OUT

IL_FAIL:
	ldc.i4 1
	ldstr "FAILED!"
IL_OUT:
	call void [mscorlib]System.Console::WriteLine(string)
	ret
}

.method public hidebysig specialname rtspecialname instance void .ctor() cil managed
{
	.maxstack  8
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor()
	ret
}
}
