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
.class public		_ckfinite {
.method public static int32 r8(float64) {
.locals init		(class [mscorlib]System.ArithmeticException,int32)
.maxstack	5
try_start:
	ldarg	0
	conv.r8
	ckfinite
	ldarg	0
	ceq
	stloc.1
	leave.s	try_end
try_end:
	ldloc.1
	brfalse		FAIL
	ldc.i4	0x11111111
	ret
aHandler:
	isinst	[mscorlib]System.ArithmeticException
	stloc	0
	leave			HEnd
HEnd:
	ldloc	0
	brfalse	FAIL
	ldc.i4	0xEEEEEEEE
	ret
FAIL:
	ldc.i4	0x0001
	ret
.try try_start to try_end catch [mscorlib]System.ArithmeticException handler aHandler to HEnd
}
.method public		void _ckfinite() {
.maxstack	0
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		5
	ldc.r8		float64(0xFFF0000000000000)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.r8		float64(0xFFEFFFFFFFFFFFFF)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.r8		float64(0xBFF0000000000000)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.r8		float64(0x8000000000000000)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.r8		float64(0x0000000000000000)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.r8		float64(0x3FF0000000000000)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.r8		float64(0x7FEFFFFFFFFFFFFF)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0x11111111
	ceq
	brfalse		FAIL
	ldc.r8		float64(0x7FF0000000000000)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
	ldc.r8		float64(0x7FF8000000000000)
	call	int32 _ckfinite::r8(float64)
	ldc.i4		0xEEEEEEEE
	ceq
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
}
}
.assembly ckfinite_r8exe{}
