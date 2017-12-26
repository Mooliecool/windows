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
.class public _ldarg {
.method public static	int32 args(float32,float32) {
.maxstack	5
	ldarg	0
	ldarg	1
	ceq
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0xFF800000)
	call	int32 _ldarg::args(float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0xFF7FFFFF)
	call	int32 _ldarg::args(float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF80000)
	ldc.r4		float32(0xBF80000)
	call	int32 _ldarg::args(float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x80000000)
	call	int32 _ldarg::args(float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x00000000)
	call	int32 _ldarg::args(float32,float32)	
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x3F800000)
	call	int32 _ldarg::args(float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x7F7FFFFF)
	call	int32 _ldarg::args(float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x7F800000)
	call	int32 _ldarg::args(float32,float32)
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly ldarg_r4exe{}
