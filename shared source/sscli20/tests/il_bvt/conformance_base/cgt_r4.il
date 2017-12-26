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
.field public static	float32 _inf
.field public static	float32 _min
.field public static	float32 _one
.field public static	float32 _zero
.field public static	float32 zero
.field public static	float32 one
.field public static	float32 max
.field public static	float32 inf
.field public static	float32 NaN
.method public static	void initialize() {
.maxstack	10
	ldc.r4		float32(0xFF800000)
	stsfld	float32 _cgt::_inf
	ldc.r4		float32(0xFF7FFFFF)
	stsfld	float32 _cgt::_min
	ldc.r4		float32(0xBF800000)
	stsfld	float32 _cgt::_one
	ldc.r4		float32(0x80000000)
	stsfld	float32 _cgt::_zero
	ldc.r4		float32(0x00000000)
	stsfld	float32 _cgt::zero
	ldc.r4		float32(0x3F800000)
	stsfld	float32 _cgt::one
	ldc.r4		float32(0x7F7FFFFF)
	stsfld	float32 _cgt::max
	ldc.r4		float32(0x7F800000)
	stsfld	float32 _cgt::inf
	ldc.r4		float32(0x7FC00000)
	stsfld	float32 _cgt::NaN
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	call	void _cgt::initialize()
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::_inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::_min
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_inf
	ldsfld	float32 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::_min
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_min
	ldsfld	float32 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_one
	ldsfld	float32 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::_zero
	ldsfld	float32 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::zero
	ldsfld	float32 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::_zero
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::one
	ldsfld	float32 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::_zero
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::one
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::max
	ldsfld	float32 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::_inf
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::_min
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::_zero
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::one
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::max
	cgt
	brfalse		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::inf
	ldsfld	float32 _cgt::NaN
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::_inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::_min
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::_zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::inf
	cgt
	brtrue		FAIL
	ldsfld	float32 _cgt::NaN
	ldsfld	float32 _cgt::NaN
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
.assembly cgt_r4exe{}
