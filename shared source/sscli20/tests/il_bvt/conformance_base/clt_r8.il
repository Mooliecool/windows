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
.class public _clt {
.field public static	float64 _inf
.field public static	float64 _min
.field public static	float64 _one
.field public static	float64 _zero
.field public static	float64 zero
.field public static	float64 one
.field public static	float64 max
.field public static	float64 inf
.field public static	float64 NaN
.method public static void initialize() {
.maxstack		10
	ldc.r8		float64(0xFFF0000000000000)
	stsfld	float64 _clt::_inf
	ldc.r8		float64(0xFF7FFFFFFFFFFFFF)
	stsfld	float64 _clt::_min
	ldc.r8		float64(0xBFF0000000000000)
	stsfld	float64 _clt::_one
	ldc.r8		float64(0x8000000000000000)
	stsfld	float64 _clt::_zero
	ldc.r8		float64(0x0000000000000000)
	stsfld	float64 _clt::zero
	ldc.r8		float64(0x3FF0000000000000)
	stsfld	float64 _clt::one
	ldc.r8		float64(0x7FEFFFFFFFFFFFFF)
	stsfld	float64 _clt::max
	ldc.r8		float64(0x7FF0000000000000)
	stsfld	float64 _clt::inf
	ldc.r8		float64(0x7FF8000000000000)
	stsfld	float64 _clt::NaN
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	call	void _clt::initialize()
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::_min
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::_one
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::_zero
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::zero
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::one
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::max
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::inf
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_inf
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::_min
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::_one
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::_zero
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::zero
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::one
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::max
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::inf
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_min
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::_min
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::_one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::_zero
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::zero
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::one
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::max
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::inf
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_one
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::_min
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::_one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::_zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::one
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::max
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::inf
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::_zero
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::_min
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::_one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::_zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::one
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::max
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::inf
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::zero
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::_min
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::_one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::_zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::max
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::inf
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::one
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::_min
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::_one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::_zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::max
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::inf
	clt
	brfalse		FAIL
	ldsfld	float64 _clt::max
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::_min
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::_one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::_zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::max
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::inf
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::_inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::_min
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::_one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::_zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::zero
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::one
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::max
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::inf
	clt
	brtrue		FAIL
	ldsfld	float64 _clt::NaN
	ldsfld	float64 _clt::NaN
	clt
	brtrue		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly clt_r8exe{}
