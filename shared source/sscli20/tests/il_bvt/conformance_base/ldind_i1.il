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
.class public ldind_i1 {
.field public static int32 LOADME
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldsflda		int32 ldind_i1::LOADME
	ldc.i4		0x12789AEF
	stind.i4
	ldc.i4		0xFFFFFFEF
	ldsflda		int32 ldind_i1::LOADME
	ldind.i1
	ceq
	brfalse		FAIL
	ldc.i4		0xFFFFFF9A
	ldsflda		int32 ldind_i1::LOADME
	ldc.i4		0x1
	add
	ldind.i1
	ceq
	brfalse		FAIL
	ldc.i4		0x00000078
	ldsflda		int32 ldind_i1::LOADME
	ldc.i4		0x2
	add
	ldind.i1
	ceq
	brfalse		FAIL
	ldc.i4		0x00000012
	ldsflda		int32 ldind_i1::LOADME
	ldc.i4		0x3
	add
	ldind.i1
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
.assembly ldind_i1exe{}
