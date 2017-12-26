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
.class public Pop4 {
.field public static int32 none
.field public static int32 all
.field public static int32 sentinel
.method public static	void initialize() {
.maxstack		20
	ldc.i4		0x00000000
	stsfld	int32 Pop4::none
	ldc.i4		0xFFFFFFFF
	stsfld	int32 Pop4::all
	ldc.i4		0xFAFB0C0D
	stsfld	int32 Pop4::sentinel
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	call	void Pop4::initialize()
	ldsfld	int32 Pop4::sentinel
	ldsfld	int32 Pop4::none
	pop
	ldsfld	int32 Pop4::sentinel
	ceq
	brfalse		FAIL
	ldsfld	int32 Pop4::sentinel
	ldsfld	int32 Pop4::all
	pop
	ldsfld	int32 Pop4::sentinel
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
.assembly pop4exe{}
