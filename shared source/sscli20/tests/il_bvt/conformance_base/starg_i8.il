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
.class public _starg {
.method public static	int32 args(int64,int64) {
.maxstack	10
.locals init	(int32)
	ldarg	0
	ldarg	1
	ldarg	0
	ldarg	1
	starg	0
	starg	1
	ldarg	0
	ceq
	stloc	0
	ldarg	1
	ceq
	ldloc	0
	add
	ldc.i4		0x2
	ceq
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	ldc.i8		0x8000000000000002
	ldc.i8		0x8000000000000000
	call	int32 _starg::args(int64,int64)
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int32 _starg::args(int64,int64)
	brfalse		FAIL
	ldc.i8		0x0000000000000002
	ldc.i8		0x0000000000000000
	call	int32 _starg::args(int64,int64)
	brfalse		FAIL
	ldc.i8		0x0000000000000003
	ldc.i8		0x0000000000000001
	call	int32 _starg::args(int64,int64)
	brfalse		FAIL
	ldc.i8		0x8000000000000001
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int32 _starg::args(int64,int64)
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly starg_i8exe{}
