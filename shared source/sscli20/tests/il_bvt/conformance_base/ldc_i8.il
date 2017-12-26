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
.class public ldc_I8 {
.field public static	int64 mix
.field public static	int64 odd
.field public static	int64 even
.field public static	int64 all
.field public static	int64 none
.method public static void initialize() {
.maxstack	10
	ldc.i8		0xFAFBFCFDFAFBFCFD
	stsfld	int64 ldc_I8::mix
	ldc.i8		0x5555555555555555
	stsfld	int64 ldc_I8::odd
	ldc.i8		0xAAAAAAAAAAAAAAAA
	stsfld	int64 ldc_I8::even
	ldc.i8		0xFFFFFFFFFFFFFFFF
	stsfld	int64 ldc_I8::all
	ldc.i8		0x0000000000000000
	stsfld	int64 ldc_I8::none
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		6
	call	void ldc_I8::initialize()
	ldc.i8		0xFAFBFCFDFAFBFCFD
	ldsfld	int64 ldc_I8::mix
	ceq
	brfalse	FAIL
	ldc.i8		0x5555555555555555
	ldsfld	int64 ldc_I8::odd
	ceq
	brfalse	FAIL
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ldsfld	int64 ldc_I8::even
	ceq
	brfalse	FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ldsfld	int64 ldc_I8::all
	ceq
	brfalse	FAIL
	ldc.i8		0x0000000000000000
	ldsfld	int64 ldc_I8::none
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
.assembly ldc_i8exe{}
