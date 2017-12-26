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
.class public Stind_i1 {
.field public static int32 FOO
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldsflda		int32 Stind_i1::FOO
	ldc.i4		0xEF
	stind.i1
	ldsflda		int32 Stind_i1::FOO
	ldc.i4		0x1
	add
	ldc.i4		0x9A
	stind.i1
	ldsflda		int32 Stind_i1::FOO
	ldc.i4		0x2
	add
	ldc.i4		0x78
	stind.i1
	ldsflda		int32 Stind_i1::FOO
	ldc.i4		0x3
	add
	ldc.i4		0x12
	stind.i1
	ldsflda		int32 Stind_i1::FOO
	ldind.i4
	ldc.i4		0x12789AEF
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
.assembly stind_i1exe{}
