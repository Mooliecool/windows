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
.class public _dup {
.field public static	int32 sentinel
.field public static	int32 none
.field public static	int32 all
.method public static	void initialize() {
.maxstack		20
	ldc.i4		0x00000000
	stsfld	int32 _dup::none
	ldc.i4		0xFFFFFFFF
	stsfld	int32 _dup::all
	ldc.i4		0xFAFB0C0D
	stsfld	int32 _dup::sentinel
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	call	void _dup::initialize()
	ldsfld	int32 _dup::sentinel
	ldsfld	int32 _dup::none
	dup
	ceq
	brfalse		FAIL
	ldsfld	int32 _dup::all
	dup
	ceq
	brfalse		FAIL
	dup
	ldsfld	int32 _dup::sentinel
	ceq
	brfalse		FAIL
	pop
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	pop
	ldc.i4	0x0
	ret
}
}
.assembly dup4exe{}
