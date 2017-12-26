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
.class public ldloc_I8 {
.field public static	int64 onlyhigh
.field public static	int64 all
.field public static	int64 none
.field public static	int64 onlylow
.field public static	int64 nothigh
.method public static	void initialize() {
.maxstack	10
	ldc.i8		0x8000000000000000
	stsfld	int64 ldloc_I8::onlyhigh
	ldc.i8		0xFFFFFFFFFFFFFFFF
	stsfld	int64 ldloc_I8::all
	ldc.i8		0x0000000000000000
	stsfld	int64 ldloc_I8::none
	ldc.i8		0x0000000000000001
	stsfld	int64 ldloc_I8::onlylow
	ldc.i8		0x7FFFFFFFFFFFFFFF
	stsfld	int64 ldloc_I8::nothigh
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
.locals init	(int64,int64,int64,int64,int64)
	call	void ldloc_I8::initialize()
	ldc.i8		0x8000000000000000
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldc.i8		0x0000000000000000
	ldc.i8		0x0000000000000001
	ldc.i8		0x7FFFFFFFFFFFFFFF
	stloc	4
	stloc	3
	stloc	2
	stloc	1
	stloc	0
	ldsfld	int64 ldloc_I8::onlyhigh
	ldloc	0
	ceq
	brfalse		FAIL
	ldsfld	int64 ldloc_I8::all
	ldloc	1
	ceq
	brfalse		FAIL
	ldsfld	int64 ldloc_I8::none
	ldloc	2
	ceq
	brfalse		FAIL
	ldsfld	int64 ldloc_I8::onlylow
	ldloc	3
	ceq
	brfalse		FAIL
	ldsfld	int64 ldloc_I8::nothigh
	ldloc	4
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
.assembly ldloc_i8exe{}
