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
.class _localloc {
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.locals	(int32)
.maxstack 10
	ldc.i4 0x1
	localloc
	stloc 0
	ldloc 0
	ldc.i4 0x8
	conv.i
	stind.i
	ldloc 0
	ldind.i
	ldc.i4 0x8
	conv.i
	bne.un FAIL
	ldc.i4 0x1
	localloc
	stloc 0
	ldloc 0
	ldc.i4 0x99
	conv.i1
	stind.i
	ldloc 0
	ldind.i1
	ldc.i4 0x99
	conv.i1
	bne.un FAIL
	ldc.i4 0x2
	localloc
	stloc 0
	ldloc 0
	ldc.i4 0xAAAA
	conv.i2
	stind.i2
	ldloc 0
	ldind.i2
	ldc.i4 0xAAAA
	conv.i2
	bne.un FAIL
	ldc.i4 0x4
	localloc
	stloc 0
	ldloc 0
	ldc.i4 0xBBBBBBBB
	stind.i4
	ldloc 0
	ldind.i4
	ldc.i4 0xBBBBBBBB
	bne.un FAIL
	ldc.i4 0x8
	localloc
	stloc 0
	ldloc 0
	ldc.i8 0xCCCCCCCCCCCCCCCC
	stind.i8
	ldloc 0
	ldind.i8
	ldc.i8 0xCCCCCCCCCCCCCCCC
	bne.un FAIL
	ldc.i4 0x4
	localloc
	stloc 0
	ldloc 0
	ldc.r4 0xDDDDDDDD
	stind.r4
	ldloc 0
	ldind.r4
	ldc.r4 0xDDDDDDDD
	bne.un FAIL
	ldc.i4 0x8
	localloc
	stloc 0
	ldloc 0
	ldc.r8 0xEEEEEEEEEEEEEEEE
	stind.r8
	ldloc 0
	ldind.r8
	ldc.r8 0xEEEEEEEEEEEEEEEE
	bne.un FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly c_localloc.exe {}
