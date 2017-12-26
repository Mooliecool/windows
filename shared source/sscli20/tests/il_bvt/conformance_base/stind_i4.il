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
.class public Stind_i4 {
.field public static int64 FOO
.field public static int64 FOO2
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldsflda		int64 Stind_i4::FOO
	ldc.i4		0x89ABCDEF
	stind.i4
	ldsflda		int64 Stind_i4::FOO
	ldc.i4		0x4
	add
	ldc.i4		0x01234567
	stind.i4
	ldsflda		int64 Stind_i4::FOO
	ldc.i4		0x8
	add
	ldc.i4		0xCCDDEEFF
	stind.i4
	ldsflda		int64 Stind_i4::FOO
	ldc.i4		0xC
	add
	ldc.i4		0x00112233
	stind.i4
	ldsflda		int64 Stind_i4::FOO
	ldind.i8
	ldc.i8		0x0123456789ABCDEF
	ceq
	brfalse		FAIL
	ldsflda		int64 Stind_i4::FOO
	ldc.i4		0x8
	add
	ldind.i8
	ldc.i8		0x00112233CCDDEEFF
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
.assembly stind_i4exe{}
