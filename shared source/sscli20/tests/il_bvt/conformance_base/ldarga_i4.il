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
.class public _ldarga {
.method public static	int32 args(int32) {
.maxstack	5
	ldarga 0
	ldind.i4
	ldc.i4 0x2
	add
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	ldc.i4		0x80000002
	ldc.i4		0x80000000
	call	int32 _ldarga::args(int32)
	ceq
	brfalse		FAIL
	ldc.i4		0x00000001
	ldc.i4		0xFFFFFFFF
	call	int32 _ldarga::args(int32)
	ceq
	brfalse		FAIL
	ldc.i4		0x00000002
	ldc.i4		0x00000000
	call	int32 _ldarga::args(int32)
	ceq
	brfalse		FAIL
	ldc.i4		0x00000003
	ldc.i4		0x00000001
	call	int32 _ldarga::args(int32)
	ceq
	brfalse		FAIL
	ldc.i4		0x80000001
	ldc.i4		0x7FFFFFFF
	call	int32 _ldarga::args(int32)
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
.assembly ldarga_i4exe{}
