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
.class public cgt_un {
.field public static	int32 all
.field public static	int32 none
.field public static	int32 odd
.field public static	int32 even
.method public static	void initialize() {
.maxstack	10
	ldc.i4		0xFFFFFFFF
	stsfld	int32 cgt_un::all
	ldc.i4		0x00000000
	stsfld	int32 cgt_un::none
	ldc.i4		0x55555555
	stsfld	int32 cgt_un::odd
	ldc.i4		0xAAAAAAAA
	stsfld	int32 cgt_un::even
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	call	void cgt_un::initialize()
	ldsfld	int32 cgt_un::all
	ldsfld	int32 cgt_un::all
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::all
	ldsfld	int32 cgt_un::none
	cgt.un
	brfalse		FAIL
	ldsfld	int32 cgt_un::all
	ldsfld	int32 cgt_un::odd
	cgt.un
	brfalse		FAIL
	ldsfld	int32 cgt_un::all
	ldsfld	int32 cgt_un::even
	cgt.un
	brfalse		FAIL
	ldsfld	int32 cgt_un::none
	ldsfld	int32 cgt_un::all
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::none
	ldsfld	int32 cgt_un::none
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::none
	ldsfld	int32 cgt_un::odd
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::none
	ldsfld	int32 cgt_un::even
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::odd
	ldsfld	int32 cgt_un::all
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::odd
	ldsfld	int32 cgt_un::none
	cgt.un
	brfalse		FAIL
	ldsfld	int32 cgt_un::odd
	ldsfld	int32 cgt_un::odd
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::odd
	ldsfld	int32 cgt_un::even
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::even
	ldsfld	int32 cgt_un::all
	cgt.un
	brtrue		FAIL
	ldsfld	int32 cgt_un::even
	ldsfld	int32 cgt_un::none
	cgt.un
	brfalse		FAIL
	ldsfld	int32 cgt_un::even
	ldsfld	int32 cgt_un::odd
	cgt.un
	brfalse		FAIL
	ldsfld	int32 cgt_un::even
	ldsfld	int32 cgt_un::even
	cgt.un
	brtrue		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly cgt_u4exe{}
