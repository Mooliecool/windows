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
.class public _cgt {
.field public static	float64 _inf
.field public static	float64 _min
.field public static	float64 _one
.field public static	float64 _zero
.field public static	float64 zero
.field public static	float64 one
.field public static	float64 max
.field public static	float64 inf
.field public static	float64 NaN
.method public static	void initialize() {
.maxstack	10
	ldc.r8		float64(0xFFF0000000000000)
	stsfld	float64 _cgt::_inf
	ldc.r8		float64(0xFF7FFFFFFFFFFFFF)
	stsfld	float64 _cgt::_min
	ldc.r8		float64(0xBFF0000000000000)
	stsfld	float64 _cgt::_one
	ldc.r8		float64(0x8000000000000000)
	stsfld	float64 _cgt::_zero
	ldc.r8		float64(0x0000000000000000)
	stsfld	float64 _cgt::zero
	ldc.r8		float64(0x3FF0000000000000)
	stsfld	float64 _cgt::one
	ldc.r8		float64(0x7FEFFFFFFFFFFFFF)
	stsfld	float64 _cgt::max
	ldc.r8		float64(0x7FF0000000000000)
	stsfld	float64 _cgt::inf
	ldc.r8		float64(0x7FF8000000000000)
	stsfld	float64 _cgt::NaN
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	call	void _cgt::initialize()
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::_inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::_min
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_inf
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::_min
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_min
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_one
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::_zero
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::zero
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::_zero
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::one
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::_zero
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::one
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::max
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::_zero
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::one
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::max
	cgt
	brfalse		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::inf
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::_inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::_min
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float64 _cgt::NaN
	ldsfld	float64 _cgt::NaN
	cgt
	brtrue		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly cgt_r8exe{}
