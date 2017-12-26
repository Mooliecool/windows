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
.class public Stind_i8 {
.field public static int64 FOO
.field public static int64 FOO2
.field public static int64 FOO3
.field public static int64 FOO4
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldsflda		int64 Stind_i8::FOO
	ldc.i8		0x0123456789ABCDEF
	stind.i8
	ldsflda		int64 Stind_i8::FOO
	ldc.i4		0x8
	add
	ldc.i8		0x0011223344556677
	stind.i8
	ldsflda		int64 Stind_i8::FOO
	ldc.i4		0x10
	add
	ldc.i8		0x8899AABBCCDDEEFF
	stind.i8
	ldsflda		int64 Stind_i8::FOO
	ldc.i4		0x18
	add
	ldc.i8		0xFFFFFFFF00000000
	stind.i8
	ldsflda		int64 Stind_i8::FOO
	ldind.i8
	ldc.i8		0x0123456789ABCDEF
	ceq
	brfalse		FAIL
	ldsflda		int64 Stind_i8::FOO
	ldc.i4		0x8
	add
	ldind.i8
	ldc.i8		0x0011223344556677
	ceq
	brfalse		FAIL	
	ldsflda		int64 Stind_i8::FOO
	ldc.i4		0x10
	add
	ldind.i8
	ldc.i8		0x8899AABBCCDDEEFF
	ceq
	brfalse		FAIL	
	ldsflda		int64 Stind_i8::FOO
	ldc.i4		0x18
	add
	ldind.i8
	ldc.i8		0xFFFFFFFF00000000
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
.assembly stind_i8exe{}
