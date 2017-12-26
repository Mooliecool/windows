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
.class public ldc_I4 {
.field public static int32 mix
.field public static int32 odd
.field public static int32 even
.field public static int32 all
.field public static int32 none
.method public static void initialize() {
.maxstack		10
	ldc.i4		0xFAFBFCFD
	stsfld	int32 ldc_I4::mix
	ldc.i4		0x55555555
	stsfld	int32 ldc_I4::odd
	ldc.i4		0xAAAAAAAA
	stsfld	int32 ldc_I4::even
	ldc.i4		0xFFFFFFFF
	stsfld	int32 ldc_I4::all
	ldc.i4		0x00000000
	stsfld	int32 ldc_I4::none
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		6
	call	void ldc_I4::initialize()
	ldc.i4		0xFAFBFCFD
	ldsfld	int32 ldc_I4::mix
	ceq
	brfalse	FAIL
	ldc.i4		0x55555555
	ldsfld	int32 ldc_I4::odd
	ceq
	brfalse	FAIL
	ldc.i4		0xAAAAAAAA
	ldsfld	int32 ldc_I4::even
	ceq
	brfalse	FAIL
	ldc.i4		0xFFFFFFFF
	ldsfld	int32 ldc_I4::all
	ceq
	brfalse	FAIL
	ldc.i4		0x00000000
	ldsfld	int32 ldc_I4::none
	ceq
	brfalse	FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly ldc_i4exe{}
