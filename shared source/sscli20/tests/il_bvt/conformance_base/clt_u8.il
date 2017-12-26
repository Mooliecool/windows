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
.class public clt_un {
.field public static	int64 all
.field public static	int64 none
.field public static	int64 odd
.field public static	int64 even
.method public static void initialize() {
.maxstack		10
	ldc.i8		0xFFFFFFFFFFFFFFFF
	stsfld		int64 clt_un::all
	ldc.i8		0x0000000000000000
	stsfld		int64 clt_un::none
	ldc.i8		0x5555555555555555
	stsfld		int64 clt_un::odd
	ldc.i8		0xAAAAAAAAAAAAAAAA
	stsfld		int64 clt_un::even
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	call		void clt_un::initialize()
	ldsfld		int64 clt_un::all
	ldsfld		int64 clt_un::all
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::all
	ldsfld		int64 clt_un::none
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::all
	ldsfld		int64 clt_un::odd
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::all
	ldsfld		int64 clt_un::even
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::none
	ldsfld		int64 clt_un::all
	clt.un
	brfalse		FAIL
	ldsfld		int64 clt_un::none
	ldsfld		int64 clt_un::none
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::none
	ldsfld		int64 clt_un::odd
	clt.un
	brfalse		FAIL
	ldsfld		int64 clt_un::none
	ldsfld		int64 clt_un::even
	clt.un
	brfalse		FAIL
	ldsfld		int64 clt_un::odd
	ldsfld		int64 clt_un::all
	clt.un
	brfalse		FAIL
	ldsfld		int64 clt_un::odd
	ldsfld		int64 clt_un::none
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::odd
	ldsfld		int64 clt_un::odd
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::odd
	ldsfld		int64 clt_un::even
	clt.un
	brfalse		FAIL
	ldsfld		int64 clt_un::even
	ldsfld		int64 clt_un::all
	clt.un
	brfalse		FAIL
	ldsfld		int64 clt_un::even
	ldsfld		int64 clt_un::none
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::even
	ldsfld		int64 clt_un::odd
	clt.un
	brtrue		FAIL
	ldsfld		int64 clt_un::even
	ldsfld		int64 clt_un::even
	clt.un
	brtrue		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly clt_u8exe{}
