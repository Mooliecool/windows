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
.class public ldind_r8 {
.field public static int64 LOADME
.field public static int64 LOADME2
.field public static int64 LOADME3
.field public static int64 LOADME4
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldsflda		int64 ldind_r8::LOADME
	ldc.r8		float64(0xFFEFFFFFFFFFFFFF)
	stind.r8
	ldsflda		int64 ldind_r8::LOADME
	ldc.i4		0x8
	add
	ldc.r8		float64(0xBFF0000000000000)
	stind.r8
	ldsflda		int64 ldind_r8::LOADME
	ldc.i4		0x10
	add
	ldc.r8		float64(0x3FF0000000000000)
	stind.r8
	ldsflda		int64 ldind_r8::LOADME
	ldc.i4		0x18
	add
	ldc.r8		float64(0x7FEFFFFFFFFFFFFF)
	stind.r8
	ldc.r8		float64(0xFFEFFFFFFFFFFFFF)
	ldsflda		int64 ldind_r8::LOADME
	ldind.r8
	ceq
	brfalse		FAIL
	ldc.r8		float64(0xBFF0000000000000)
	ldsflda		int64 ldind_r8::LOADME
	ldc.i4		0x8
	add
	ldind.r8
	ceq
	brfalse		FAIL
	ldc.r8		float64(0x3FF0000000000000)
	ldsflda		int64 ldind_r8::LOADME
	ldc.i4		0x10
	add
	ldind.r8
	ceq
	brfalse		FAIL
	ldc.r8		float64(0x7FEFFFFFFFFFFFFF)
	ldsflda		int64 ldind_r8::LOADME
	ldc.i4		0x18
	add
	ldind.r8
	ceq
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly ldind_r8exe{}
