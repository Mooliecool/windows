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
.class public _sizeof {
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	sizeof int8
	ldc.i4 0x1
	bne.un FAIL
	sizeof int16
	ldc.i4 0x2
	bne.un FAIL
	sizeof int32
	ldc.i4 0x4
	bne.un FAIL
	sizeof int64
	ldc.i4 0x8
	bne.un FAIL
	sizeof unsigned int8
	ldc.i4 0x1
	bne.un FAIL
	sizeof unsigned int16
	ldc.i4 0x2
	bne.un FAIL
	sizeof unsigned int32
	ldc.i4 0x4
	bne.un FAIL
	sizeof unsigned int64
	ldc.i4 0x8
	bne.un FAIL
	sizeof float32
	ldc.i4 0x4
	bne.un FAIL
	sizeof float64
	ldc.i4 0x8
	bne.un FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly sizeofexe{}
