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
.assembly 'refany' { }
.class public auto ansi beforefieldinit Test extends [mscorlib]System.Object {
.method public hidebysig instance void Negative1() cil managed
{
	.maxstack  8
	.locals init (object)
	.zeroinit
	ldloca 0
	mkrefany string
	pop
	ret
}
.method public hidebysig instance void Negative2() cil managed
{
	.maxstack  8
	.locals init (int32)
	.zeroinit
	ldloca 0
	mkrefany int64
	pop
	ret
}
.method public hidebysig instance void PevNegative3() cil managed
{
	.maxstack  8
	.locals init (int32)
	ldc.i4 10
	newarr int32
	ldc.i4.3
	ldelema int32
	mkrefany native unsigned int
	pop
	ret
}
.method public hidebysig instance void PevNegative4() cil managed
{
	.maxstack  8
	.locals init (int32)
	.zeroinit
	ldloca 0
	mkrefany native int
	pop
	ret
}
.method public hidebysig instance void Negative5() cil managed
{
	.maxstack  8
	.locals init (float32)
	.zeroinit
	ldloca 0
	mkrefany float64
	pop
	ret
}
.method public hidebysig instance void Negative6() cil managed
{
	.maxstack  8
	.locals init (float64)
	.zeroinit
	ldloca 0
	mkrefany float32
	pop
	ret
}
.method public hidebysig instance void Positive7() cil managed
{
	.maxstack  8
	.locals init (int64)
	.zeroinit
	ldloca 0
	mkrefany unsigned int64
	pop
	ret
}
.method public hidebysig instance void Negative8() cil managed
{
	.maxstack  8
	.locals init (int8&)
	.zeroinit
	ldloc.0
	mkrefany int16
	pop
	ret
}
.method public hidebysig instance void Negative9() cil managed
{
	.maxstack  8
	.locals init (int32&)
	.zeroinit
	ldloca 0
	mkrefany int32&
	pop
	ret
}
.method public hidebysig instance void Negative10() cil managed
{
	.maxstack  8
	.locals init (int32&)
	.zeroinit
	ldloc.0
	refanyval int32
	pop
	ret
}
.method public hidebysig instance void Negative11() cil managed
{
	.maxstack  8
	.locals init (unsigned int64)
	.zeroinit
	ldloc.0
	refanytype
	pop
	ret
}
.method public hidebysig instance void Positive12() cil managed
{
	.maxstack  8
	.locals init (float32)
	.zeroinit
	.try {
		ldloca.s 0
		mkrefany float32
		refanyval float64
		ldind.r8
		leave.s IL_END
	} catch [mscorlib]System.InvalidCastException {
		leave.s IL_END
	}
IL_END:
	ret
}
.method public hidebysig instance void Negative13() cil managed
{
	.maxstack  8
	.locals init (float32[])
	ldc.i4.7
	newarr float32
	stloc.0
	ldloca.s 0
	mkrefany float64[]
	pop
	ret
}
.method public hidebysig instance void Negative14() cil managed
{
	.maxstack  8
	.locals init (float32[0...,0...])
	ldc.i4.0
	ldc.i4.4
	ldc.i4.0
	ldc.i4.4
	newobj instance void float32[0...,0...]::.ctor(int32, int32, int32, int32)
	stloc.0
	ldloca.s 0
	mkrefany float64[0...,0...]
	pop
	ret
}
.method public hidebysig instance void Negative15() cil managed
{
	.maxstack  8
	ldc.r4 0
	box float64
	mkrefany float64
	pop
	ret
}
.method public hidebysig instance void Negative16() cil managed
{
	.maxstack  8
	.locals init (int32)
	ldloca.s 0
	.emitbyte 0xc6
	.emitbyte 0xff
	.emitbyte 0xff
	.emitbyte 0xff
	.emitbyte 0xff
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
	ldloc.0
	callvirt instance void Test::PevNegative3()
IL_NEXT_3:
	ldloc.0
	callvirt instance void Test::PevNegative4()
IL_NEXT_4:
	.try {
		ldloc.0
		callvirt instance void Test::Negative5()
		ldstr "Negative5() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_5
	}
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
	.try {
		ldloc.0
		callvirt instance void Test::Negative11()
		ldstr "Negative11() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_11
	}
IL_NEXT_11:
	ldloc.0
	callvirt instance void Test::Positive12()
IL_NEXT_12:
	.try {
		ldloc.0
		callvirt instance void Test::Negative13()
		ldstr "Negative13() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_13
	}
IL_NEXT_13:
	.try {
		ldloc.0
		callvirt instance void Test::Negative14()
		ldstr "Negative14() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_14
	}
IL_NEXT_14:
	.try {
		ldloc.0
		callvirt instance void Test::Negative15()
		ldstr "Negative15() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_15
	}
IL_NEXT_15:
	.try {
		ldloc.0
		callvirt instance void Test::Negative16()
		ldstr "Negative16() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_16
	}
IL_NEXT_16:
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
