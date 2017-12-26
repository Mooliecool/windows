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
.class public _starg {
.method public static	int32 args(float64,float64) {
.maxstack	10
.locals init	(int32)
	ldarg	0
	ldarg	1
	ldarg	0
	ldarg	1
	starg	0
	starg	1
	ldarg	0
	ceq
	stloc	0
	ldarg	1
	ceq
	ldloc	0
	add
	ldc.i4		0x2
	ceq
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	ldc.r8		float64(0x3FF7FFFFFFFFFFFF)
	ldc.r8		float64(0xFFF0000000000000)
	call	int32 _starg::args(float64,float64)
	brfalse		FAIL
	ldc.r8		float64(0x3FF7FFFFFFFFFFFF)
	ldc.r8		float64(0xFFEFFFFFFFFFFFFF)
	call	int32 _starg::args(float64,float64)
	brfalse		FAIL
	ldc.r8		float64(0x3FF7FFFFFFFFFFFF)
	ldc.r8		float64(0xBFF0000000000000)
	call	int32 _starg::args(float64,float64)
	brfalse		FAIL
	ldc.r8		float64(0x3FF7FFFFFFFFFFFF)
	ldc.r8		float64(0x8000000000000000)
	call	int32 _starg::args(float64,float64)
	brfalse		FAIL
	ldc.r8		float64(0x3FF7FFFFFFFFFFFF)
	ldc.r8		float64(0x0000000000000000)
	call	int32 _starg::args(float64,float64)	
	brfalse		FAIL
	ldc.r8		float64(0x3FF7FFFFFFFFFFFF)
	ldc.r8		float64(0x3FF0000000000000)
	call	int32 _starg::args(float64,float64)
	brfalse		FAIL
	ldc.r8		float64(0x3FF7FFFFFFFFFFFF)
	ldc.r8		float64(0x7FEFFFFFFFFFFFFF)
	call	int32 _starg::args(float64,float64)
	brfalse		FAIL
	ldc.r8		float64(0x3FF7FFFFFFFFFFFF)
	ldc.r8		float64(0x7FF0000000000000)
	call	int32 _starg::args(float64,float64)
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly starg_r8exe{}
