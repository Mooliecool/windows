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
.assembly 'box' { }
.class public auto ansi beforefieldinit Test extends [mscorlib]System.Object {
.method public hidebysig instance void Negative1() cil managed
{
	.maxstack  8
	ldc.i4.7
	.emitbyte 0x8c
	.emitbyte 0xff
	.emitbyte 0xff
	.emitbyte 0xff
	.emitbyte 0xff
	pop
	ret
}
.method public hidebysig instance void Negative2() cil managed
{
	.maxstack  8
	ldc.i4.7
	.emitbyte 0x8c
	.emitbyte 0x01
	.emitbyte 0xff
	.emitbyte 0xff
	.emitbyte 0x01
	pop
	ret
}
.method public hidebysig instance void Positive3() cil managed
{
	.maxstack  8
	ldarg.0
	box class Test
	pop
	ret
}
.method public hidebysig instance void Negative4() cil managed
{
	.maxstack  8
	ldc.i4.6
	box unsigned int32
	box unsigned int32
	pop
	ret
}
.method public hidebysig instance void Positive5() cil managed
{
	.maxstack  8
	.try {
		ldc.i8 111
		box int64
		unbox float64
		ldind.r8
		call void [mscorlib]System.Console::WriteLine(float64)
		leave.s IL_END
	} catch [mscorlib]System.InvalidCastException {
		leave.s IL_END
	}
IL_END:
	ret
}
.method public hidebysig instance void Negative6() cil managed
{
	.maxstack  8
	.locals init (refany)
	.zeroinit
	ldloc.0
	box refany
	pop
	ret
}
.method public hidebysig instance void Positive7() cil managed
{
	.maxstack  8
	.try {
		ldarg.0
		unbox float64
		ldind.r8
		call void [mscorlib]System.Console::WriteLine(float64)
		leave.s IL_END
	} catch [mscorlib]System.InvalidCastException {
		leave.s IL_END
	}
IL_END:
	ret
}
.method public hidebysig instance void Negative8() cil managed
{
	.maxstack  8
	.locals init (float32)
	.zeroinit
	ldloc.0
	box int32
	pop
	ret
}
.method public hidebysig instance void Negative9() cil managed
{
	.maxstack  8
	.locals init (float32)
	.zeroinit
	ldloca 0
	box int32
	pop
	ret
}
.method public hidebysig instance void Negative10() cil managed
{
	.maxstack  8
	ldnull
	box int32
	pop
	ret
}
.method public hidebysig instance void Positive11() cil managed
{
	.maxstack  8
	.try {
		ldnull
		unbox float64
		ldind.r8
		call void [mscorlib]System.Console::WriteLine(float64)
		leave.s IL_END
	} catch [mscorlib]System.NullReferenceException {
		leave.s IL_END
	}
IL_END:
	ret
}
.method public hidebysig instance void Positive12() cil managed
{
	.maxstack  8
	.try {
		ldc.i4.1
		newarr int32
		unbox float64
		ldind.r8
		call void [mscorlib]System.Console::WriteLine(float64)
		leave.s IL_END
	} catch [mscorlib]System.InvalidCastException {
		leave.s IL_END
	}
IL_END:
	ret
}
.method public hidebysig static int32 Main() cil managed
{
	.entrypoint
	.maxstack 8
	.locals init (class Test V_0)
	newobj     instance void Test::.ctor()
	stloc.0
	.try {
		ldloc.0
		callvirt instance void Test::Negative1()
		ldstr "Negative1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_1
	}
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
	.try {
		ldloc.0
		callvirt instance void Test::Positive3()
		leave.s IL_NEXT_3
	} catch [mscorlib]System.Security.VerificationException {
                ldstr "Positive3() failed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	}
IL_NEXT_3:
	.try {
		ldloc.0
		callvirt instance void Test::Negative4()
		ldstr "Negative4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_4
	}
IL_NEXT_4:
	ldloc.0
	callvirt instance void Test::Positive5()
IL_NEXT_5:
	.try {
		ldloc.0
		callvirt instance void Test::Negative6()
		ldstr "Negative6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_6
	}
IL_NEXT_6:
	ldloc.0
	callvirt instance void Test::Positive7()
IL_NEXT_7:
	.try {
		ldloc.0
		callvirt instance void Test::Negative8()
		ldstr "Negative8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_8
	}
IL_NEXT_8:
	.try {
		ldloc.0
		callvirt instance void Test::Negative9()
		ldstr "Negative9() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_9
	}
IL_NEXT_9:
	.try {
		ldloc.0
		callvirt instance void Test::Negative10()
		ldstr "Negative10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_10
	}
IL_NEXT_10:
	ldloc.0
	callvirt instance void Test::Positive11()
IL_NEXT_11:
	ldloc.0
	callvirt instance void Test::Positive12()
IL_NEXT_12:
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
