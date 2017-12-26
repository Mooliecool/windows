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
.class public Stind_r4 {
.field public static int64 LOADME
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldsflda		int64 Stind_r4::LOADME
	ldc.r4		float32(0xFF7FFFFF)
	stind.r4
	ldsflda		int64 Stind_r4::LOADME
	ldc.i4		0x4
	add
	ldc.r4		float32(0x7F7FFFFF)
	stind.r4
	ldsflda		int64 Stind_r4::LOADME
	ldc.i4		0x8
	add
	ldc.r4		float32(0xBF800000)
	stind.r4
	ldsflda		int64 Stind_r4::LOADME
	ldc.i4		0xC
	add
	ldc.r4		float32(0x3F800000)
	stind.r4
	ldc.r4		float32(0xFF7FFFFF)
	ldsflda		int64 Stind_r4::LOADME
	ldind.r4
	ceq
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldsflda		int64 Stind_r4::LOADME
	ldc.i4		0x4
	add
	ldind.r4
	ceq
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldsflda		int64 Stind_r4::LOADME
	ldc.i4		0x8
	add
	ldind.r4
	ceq
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldsflda		int64 Stind_r4::LOADME
	ldc.i4		0xC
	add
	ldind.r4
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
.assembly stind_r4exe{}
