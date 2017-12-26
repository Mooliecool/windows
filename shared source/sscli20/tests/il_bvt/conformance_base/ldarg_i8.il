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
.class public _ldarg {
.method public static	int64 args(int64) {
.maxstack	5
	ldarg	0
	ldc.i8		0x2
	add
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	ldc.i8		0x8000000000000002
	ldc.i8		0x8000000000000000
	call	int64 _ldarg::args(int64)
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _ldarg::args(int64)
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000002
	ldc.i8		0x0000000000000000
	call	int64 _ldarg::args(int64)
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000003
	ldc.i8		0x0000000000000001
	call	int64 _ldarg::args(int64)
	ceq
	brfalse		FAIL
	ldc.i8		0x8000000000000001
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _ldarg::args(int64)
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
.assembly ldarg_i8exe{}
