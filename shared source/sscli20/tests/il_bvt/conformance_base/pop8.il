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
.class public Pop8 {
.field public static int64 none
.field public static int64 all
.field public static int32 sentinel
.method public static	void initialize() {
.maxstack		20
	ldc.i8		0x0000000000000000
	stsfld	int64 Pop8::none
	ldc.i8		0xFFFFFFFFFFFFFFFF
	stsfld	int64 Pop8::all
	ldc.i4		0xFAFB0C0D
	stsfld	int32 Pop8::sentinel
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	call	void Pop8::initialize()
	ldsfld	int32 Pop8::sentinel
	ldsfld	int64 Pop8::none
	pop
	ldsfld	int32 Pop8::sentinel
	ceq
	brfalse		FAIL
	ldsfld	int32 Pop8::sentinel
	ldsfld	int64 Pop8::all
	pop
	ldsfld	int32 Pop8::sentinel
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
.assembly pop8exe{}
