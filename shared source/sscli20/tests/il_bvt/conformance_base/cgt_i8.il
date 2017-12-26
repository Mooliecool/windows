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
.field public static	int64 min
.field public static	int64 _one
.field public static	int64 zero
.field public static	int64 one
.field public static	int64 max
.field public static	int64 odd
.field public static	int64 even
.method public static	void initialize() {
.maxstack	10
	ldc.i8		0x8000000000000000
	stsfld	int64 _cgt::min
	ldc.i8		0xFFFFFFFFFFFFFFFF
	stsfld	int64 _cgt::_one
	ldc.i8		0x0000000000000000
	stsfld	int64 _cgt::zero
	ldc.i8		0x0000000000000001
	stsfld	int64 _cgt::one
	ldc.i8		0x7FFFFFFFFFFFFFFF
	stsfld	int64 _cgt::max
	ldc.i8		0x5555555555555555
	stsfld	int64 _cgt::odd
	ldc.i8		0xAAAAAAAAAAAAAAAA
	stsfld	int64 _cgt::even
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	call	void _cgt::initialize()
	ldsfld	int64 _cgt::min
	ldsfld	int64 _cgt::min
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::min
	ldsfld	int64 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::min
	ldsfld	int64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::min
	ldsfld	int64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::min
	ldsfld	int64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::min
	ldsfld	int64 _cgt::odd
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::min
	ldsfld	int64 _cgt::even
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::_one
	ldsfld	int64 _cgt::min
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::_one
	ldsfld	int64 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::_one
	ldsfld	int64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::_one
	ldsfld	int64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::_one
	ldsfld	int64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::_one
	ldsfld	int64 _cgt::odd
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::_one
	ldsfld	int64 _cgt::even
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::zero
	ldsfld	int64 _cgt::min
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::zero
	ldsfld	int64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::zero
	ldsfld	int64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::zero
	ldsfld	int64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::zero
	ldsfld	int64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::zero
	ldsfld	int64 _cgt::odd
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::zero
	ldsfld	int64 _cgt::even
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::one
	ldsfld	int64 _cgt::min
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::one
	ldsfld	int64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::one
	ldsfld	int64 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::one
	ldsfld	int64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::one
	ldsfld	int64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::one
	ldsfld	int64 _cgt::odd
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::one
	ldsfld	int64 _cgt::even
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::max
	ldsfld	int64 _cgt::min
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::max
	ldsfld	int64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::max
	ldsfld	int64 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::max
	ldsfld	int64 _cgt::one
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::max
	ldsfld	int64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::max
	ldsfld	int64 _cgt::odd
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::max
	ldsfld	int64 _cgt::even
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::odd
	ldsfld	int64 _cgt::min
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::odd
	ldsfld	int64 _cgt::_one
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::odd
	ldsfld	int64 _cgt::zero
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::odd
	ldsfld	int64 _cgt::one
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::odd
	ldsfld	int64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::odd
	ldsfld	int64 _cgt::odd
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::odd
	ldsfld	int64 _cgt::even
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::even
	ldsfld	int64 _cgt::min
	cgt
	brfalse		FAIL
	ldsfld	int64 _cgt::even
	ldsfld	int64 _cgt::_one
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::even
	ldsfld	int64 _cgt::zero
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::even
	ldsfld	int64 _cgt::one
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::even
	ldsfld	int64 _cgt::max
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::even
	ldsfld	int64 _cgt::odd
	cgt
	brtrue		FAIL
	ldsfld	int64 _cgt::even
	ldsfld	int64 _cgt::even
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
.assembly cgt_i8exe{}
