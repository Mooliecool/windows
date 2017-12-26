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
.assembly 'starg' { }
.class public auto ansi beforefieldinit Test extends [mscorlib]System.Object {
.method public hidebysig instance void Positive_I4_0_0(int8 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I4_1_0(int8 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_0(int8 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_0(int8 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_0(int8 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_0(int8 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_0(int8 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_0(int8 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I4_0_1(unsigned int8 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I4_1_1(unsigned int8 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_1(unsigned int8 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_1(unsigned int8 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_1(unsigned int8 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_1(unsigned int8 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_1(unsigned int8 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_1(unsigned int8 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I4_0_2(int16 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I4_1_2(int16 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_2(int16 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_2(int16 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_2(int16 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_2(int16 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_2(int16 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_2(int16 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I4_0_3(unsigned int16 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I4_1_3(unsigned int16 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_3(unsigned int16 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_3(unsigned int16 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_3(unsigned int16 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_3(unsigned int16 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_3(unsigned int16 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_3(unsigned int16 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I4_0_4(int32 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I4_1_4(int32 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_4(int32 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_4(int32 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_4(int32 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_4(int32 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_4(int32 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_4(int32 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I4_0_5(unsigned int32 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I4_1_5(unsigned int32 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_5(unsigned int32 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_5(unsigned int32 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_5(unsigned int32 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_5(unsigned int32 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_5(unsigned int32 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_5(unsigned int32 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I4_0_6(native int ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I4_1_6(native int ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_6(native int ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_6(native int ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_6(native int ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_6(native int ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_6(native int ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_6(native int ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I4_0_7(native unsigned int ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I4_1_7(native unsigned int ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_7(native unsigned int ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_7(native unsigned int ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_7(native unsigned int ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_7(native unsigned int ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_7(native unsigned int ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_7(native unsigned int ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I4_0_8(int64 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I4_1_8(int64 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I8_0_8(int64 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I8_1_8(int64 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_8(int64 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_8(int64 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_8(int64 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_8(int64 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I4_0_9(unsigned int64 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I4_1_9(unsigned int64 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Positive_I8_0_9(unsigned int64 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_I8_1_9(unsigned int64 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_9(unsigned int64 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_9(unsigned int64 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_9(unsigned int64 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_9(unsigned int64 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I4_0_10(float32 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I4_1_10(float32 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_10(float32 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_10(float32 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Positive_R_0_10(float32 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_R_1_10(float32 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_10(float32 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_10(float32 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I4_0_11(float64 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I4_1_11(float64 ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_11(float64 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_11(float64 ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Positive_R_0_11(float64 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_R_1_11(float64 ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_REF_0_11(float64 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_REF_1_11(float64 ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I4_0_12(string ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I4_1_12(string ARG) cil managed
{
	.maxstack 8
	ldc.i4.0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_I8_0_12(string ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_I8_1_12(string ARG) cil managed
{
	.maxstack 8
	ldc.i8 0
	starg ARG
	ret
}
.method public hidebysig instance void Negative_R_0_12(string ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg.s ARG
	ret
}
.method public hidebysig instance void Negative_R_1_12(string ARG) cil managed
{
	.maxstack 8
	ldc.r8 0
	starg ARG
	ret
}
.method public hidebysig instance void Positive_REF_0_12(string ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg.s ARG
	ret
}
.method public hidebysig instance void Positive_REF_1_12(string ARG) cil managed
{
	.maxstack 8
	ldstr "Hi"
	starg ARG
	ret
}.method public hidebysig static int32 Main() cil managed
{
	.entrypoint
	.maxstack 8
	.locals init (class Test V_0, int32)
	.zeroinit
	newobj     instance void Test::.ctor()
	stloc.0
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_0_0(int8)
IL_NEXT_1:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_1_0(int8)
IL_NEXT_2:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_0_0(int8)
		ldstr "Negative_I8_0_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_3
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_3
	}
IL_NEXT_3:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_1_0(int8)
		ldstr "Negative_I8_1_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_4
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_4
	}
IL_NEXT_4:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_0_0(int8)
		ldstr "Negative_R_0_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_5
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_5
	}
IL_NEXT_5:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_1_0(int8)
		ldstr "Negative_R_1_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_6
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_6
	}
IL_NEXT_6:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_0_0(int8)
		ldstr "Negative_REF_0_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_7
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_7
	}
IL_NEXT_7:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_1_0(int8)
		ldstr "Negative_REF_1_0() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_8
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_8
	}
IL_NEXT_8:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_0_1(unsigned int8)
IL_NEXT_9:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_1_1(unsigned int8)
IL_NEXT_10:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_0_1(unsigned int8)
		ldstr "Negative_I8_0_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_11
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_11
	}
IL_NEXT_11:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_1_1(unsigned int8)
		ldstr "Negative_I8_1_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_12
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_12
	}
IL_NEXT_12:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_0_1(unsigned int8)
		ldstr "Negative_R_0_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_13
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_13
	}
IL_NEXT_13:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_1_1(unsigned int8)
		ldstr "Negative_R_1_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_14
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_14
	}
IL_NEXT_14:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_0_1(unsigned int8)
		ldstr "Negative_REF_0_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_15
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_15
	}
IL_NEXT_15:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_1_1(unsigned int8)
		ldstr "Negative_REF_1_1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_16
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_16
	}
IL_NEXT_16:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_0_2(int16)
IL_NEXT_17:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_1_2(int16)
IL_NEXT_18:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_0_2(int16)
		ldstr "Negative_I8_0_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_19
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_19
	}
IL_NEXT_19:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_1_2(int16)
		ldstr "Negative_I8_1_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_20
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_20
	}
IL_NEXT_20:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_0_2(int16)
		ldstr "Negative_R_0_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_21
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_21
	}
IL_NEXT_21:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_1_2(int16)
		ldstr "Negative_R_1_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_22
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_22
	}
IL_NEXT_22:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_0_2(int16)
		ldstr "Negative_REF_0_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_23
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_23
	}
IL_NEXT_23:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_1_2(int16)
		ldstr "Negative_REF_1_2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_24
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_24
	}
IL_NEXT_24:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_0_3(unsigned int16)
IL_NEXT_25:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_1_3(unsigned int16)
IL_NEXT_26:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_0_3(unsigned int16)
		ldstr "Negative_I8_0_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_27
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_27
	}
IL_NEXT_27:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_1_3(unsigned int16)
		ldstr "Negative_I8_1_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_28
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_28
	}
IL_NEXT_28:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_0_3(unsigned int16)
		ldstr "Negative_R_0_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_29
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_29
	}
IL_NEXT_29:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_1_3(unsigned int16)
		ldstr "Negative_R_1_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_30
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_30
	}
IL_NEXT_30:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_0_3(unsigned int16)
		ldstr "Negative_REF_0_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_31
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_31
	}
IL_NEXT_31:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_1_3(unsigned int16)
		ldstr "Negative_REF_1_3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_32
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_32
	}
IL_NEXT_32:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_0_4(int32)
IL_NEXT_33:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_1_4(int32)
IL_NEXT_34:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_0_4(int32)
		ldstr "Negative_I8_0_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_35
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_35
	}
IL_NEXT_35:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_1_4(int32)
		ldstr "Negative_I8_1_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_36
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_36
	}
IL_NEXT_36:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_0_4(int32)
		ldstr "Negative_R_0_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_37
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_37
	}
IL_NEXT_37:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_1_4(int32)
		ldstr "Negative_R_1_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_38
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_38
	}
IL_NEXT_38:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_0_4(int32)
		ldstr "Negative_REF_0_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_39
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_39
	}
IL_NEXT_39:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_1_4(int32)
		ldstr "Negative_REF_1_4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_40
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_40
	}
IL_NEXT_40:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_0_5(unsigned int32)
IL_NEXT_41:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_1_5(unsigned int32)
IL_NEXT_42:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_0_5(unsigned int32)
		ldstr "Negative_I8_0_5() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_43
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_43
	}
IL_NEXT_43:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_1_5(unsigned int32)
		ldstr "Negative_I8_1_5() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_44
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_44
	}
IL_NEXT_44:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_0_5(unsigned int32)
		ldstr "Negative_R_0_5() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_45
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_45
	}
IL_NEXT_45:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_1_5(unsigned int32)
		ldstr "Negative_R_1_5() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_46
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_46
	}
IL_NEXT_46:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_0_5(unsigned int32)
		ldstr "Negative_REF_0_5() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_47
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_47
	}
IL_NEXT_47:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_1_5(unsigned int32)
		ldstr "Negative_REF_1_5() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_48
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_48
	}
IL_NEXT_48:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_0_6(native int)
IL_NEXT_49:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_1_6(native int)
IL_NEXT_50:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_0_6(native int)
		ldstr "Negative_I8_0_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_51
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_51
	}
IL_NEXT_51:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_1_6(native int)
		ldstr "Negative_I8_1_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_52
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_52
	}
IL_NEXT_52:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_0_6(native int)
		ldstr "Negative_R_0_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_53
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_53
	}
IL_NEXT_53:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_1_6(native int)
		ldstr "Negative_R_1_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_54
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_54
	}
IL_NEXT_54:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_0_6(native int)
		ldstr "Negative_REF_0_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_55
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_55
	}
IL_NEXT_55:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_1_6(native int)
		ldstr "Negative_REF_1_6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_56
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_56
	}
IL_NEXT_56:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_0_7(native unsigned int)
IL_NEXT_57:
	ldloc.0
	ldc.i4.0
	callvirt instance void Test::Positive_I4_1_7(native unsigned int)
IL_NEXT_58:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_0_7(native unsigned int)
		ldstr "Negative_I8_0_7() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_59
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_59
	}
IL_NEXT_59:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_I8_1_7(native unsigned int)
		ldstr "Negative_I8_1_7() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_60
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_60
	}
IL_NEXT_60:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_0_7(native unsigned int)
		ldstr "Negative_R_0_7() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_61
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_61
	}
IL_NEXT_61:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_R_1_7(native unsigned int)
		ldstr "Negative_R_1_7() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_62
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_62
	}
IL_NEXT_62:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_0_7(native unsigned int)
		ldstr "Negative_REF_0_7() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_63
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_63
	}
IL_NEXT_63:
	.try {
		ldloc.0
		ldc.i4.0
		callvirt instance void Test::Negative_REF_1_7(native unsigned int)
		ldstr "Negative_REF_1_7() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_64
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_64
	}
IL_NEXT_64:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_I4_0_8(int64)
		ldstr "Negative_I4_0_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_65
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_65
	}
IL_NEXT_65:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_I4_1_8(int64)
		ldstr "Negative_I4_1_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_66
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_66
	}
IL_NEXT_66:
	ldloc.0
	ldc.i8 0
	callvirt instance void Test::Positive_I8_0_8(int64)
IL_NEXT_67:
	ldloc.0
	ldc.i8 0
	callvirt instance void Test::Positive_I8_1_8(int64)
IL_NEXT_68:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_R_0_8(int64)
		ldstr "Negative_R_0_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_69
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_69
	}
IL_NEXT_69:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_R_1_8(int64)
		ldstr "Negative_R_1_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_70
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_70
	}
IL_NEXT_70:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_REF_0_8(int64)
		ldstr "Negative_REF_0_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_71
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_71
	}
IL_NEXT_71:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_REF_1_8(int64)
		ldstr "Negative_REF_1_8() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_72
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_72
	}
IL_NEXT_72:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_I4_0_9(unsigned int64)
		ldstr "Negative_I4_0_9() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_73
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_73
	}
IL_NEXT_73:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_I4_1_9(unsigned int64)
		ldstr "Negative_I4_1_9() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_74
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_74
	}
IL_NEXT_74:
	ldloc.0
	ldc.i8 0
	callvirt instance void Test::Positive_I8_0_9(unsigned int64)
IL_NEXT_75:
	ldloc.0
	ldc.i8 0
	callvirt instance void Test::Positive_I8_1_9(unsigned int64)
IL_NEXT_76:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_R_0_9(unsigned int64)
		ldstr "Negative_R_0_9() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_77
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_77
	}
IL_NEXT_77:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_R_1_9(unsigned int64)
		ldstr "Negative_R_1_9() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_78
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_78
	}
IL_NEXT_78:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_REF_0_9(unsigned int64)
		ldstr "Negative_REF_0_9() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_79
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_79
	}
IL_NEXT_79:
	.try {
		ldloc.0
		ldc.i8 0
		callvirt instance void Test::Negative_REF_1_9(unsigned int64)
		ldstr "Negative_REF_1_9() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_80
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_80
	}
IL_NEXT_80:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_I4_0_10(float32)
		ldstr "Negative_I4_0_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_81
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_81
	}
IL_NEXT_81:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_I4_1_10(float32)
		ldstr "Negative_I4_1_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_82
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_82
	}
IL_NEXT_82:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_I8_0_10(float32)
		ldstr "Negative_I8_0_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_83
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_83
	}
IL_NEXT_83:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_I8_1_10(float32)
		ldstr "Negative_I8_1_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_84
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_84
	}
IL_NEXT_84:
	ldloc.0
	ldc.r8 0
	callvirt instance void Test::Positive_R_0_10(float32)
IL_NEXT_85:
	ldloc.0
	ldc.r8 0
	callvirt instance void Test::Positive_R_1_10(float32)
IL_NEXT_86:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_REF_0_10(float32)
		ldstr "Negative_REF_0_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_87
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_87
	}
IL_NEXT_87:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_REF_1_10(float32)
		ldstr "Negative_REF_1_10() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_88
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_88
	}
IL_NEXT_88:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_I4_0_11(float64)
		ldstr "Negative_I4_0_11() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_89
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_89
	}
IL_NEXT_89:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_I4_1_11(float64)
		ldstr "Negative_I4_1_11() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_90
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_90
	}
IL_NEXT_90:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_I8_0_11(float64)
		ldstr "Negative_I8_0_11() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_91
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_91
	}
IL_NEXT_91:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_I8_1_11(float64)
		ldstr "Negative_I8_1_11() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_92
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_92
	}
IL_NEXT_92:
	ldloc.0
	ldc.r8 0
	callvirt instance void Test::Positive_R_0_11(float64)
IL_NEXT_93:
	ldloc.0
	ldc.r8 0
	callvirt instance void Test::Positive_R_1_11(float64)
IL_NEXT_94:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_REF_0_11(float64)
		ldstr "Negative_REF_0_11() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_95
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_95
	}
IL_NEXT_95:
	.try {
		ldloc.0
		ldc.r8 0
		callvirt instance void Test::Negative_REF_1_11(float64)
		ldstr "Negative_REF_1_11() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_96
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_96
	}
IL_NEXT_96:
	.try {
		ldloc.0
		ldstr "Hi"
		callvirt instance void Test::Negative_I4_0_12(string)
		ldstr "Negative_I4_0_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_97
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_97
	}
IL_NEXT_97:
	.try {
		ldloc.0
		ldstr "Hi"
		callvirt instance void Test::Negative_I4_1_12(string)
		ldstr "Negative_I4_1_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_98
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_98
	}
IL_NEXT_98:
	.try {
		ldloc.0
		ldstr "Hi"
		callvirt instance void Test::Negative_I8_0_12(string)
		ldstr "Negative_I8_0_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_99
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_99
	}
IL_NEXT_99:
	.try {
		ldloc.0
		ldstr "Hi"
		callvirt instance void Test::Negative_I8_1_12(string)
		ldstr "Negative_I8_1_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_100
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_100
	}
IL_NEXT_100:
	.try {
		ldloc.0
		ldstr "Hi"
		callvirt instance void Test::Negative_R_0_12(string)
		ldstr "Negative_R_0_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_101
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_101
	}
IL_NEXT_101:
	.try {
		ldloc.0
		ldstr "Hi"
		callvirt instance void Test::Negative_R_1_12(string)
		ldstr "Negative_R_1_12() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		ldc.i4.1
		stloc.1
		leave.s IL_NEXT_102
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_102
	}
IL_NEXT_102:
	ldloc.0
	ldstr "Hi"
	callvirt instance void Test::Positive_REF_0_12(string)
IL_NEXT_103:
	ldloc.0
	ldstr "Hi"
	callvirt instance void Test::Positive_REF_1_12(string)
IL_NEXT_104:
	ldloc.1
	brtrue IL_FAIL
	ldstr "PASSED"
	br.s IL_OUT
IL_FAIL:
	ldstr "FAILED!"
IL_OUT:
	call void [mscorlib]System.Console::WriteLine(string)
	ldloc.1
	ldc.i4 0
	add
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
