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
.class public _rem {
.method public static	int32 _rem(float32,float32,float32) {
.maxstack		3
.locals init	(int32)
	ldarg	0
	ldarg	1
	rem
	dup
	call	int32 _rem::checkfinite(float32)
	ldc.i4		0x0
	ceq
	brtrue		COMPARE
	call	int32 _rem::checkfinite(float32)
	stloc	0	
	ldloc	0	
	ldc.i4		0xFF
	ceq
	brtrue		FAIL	
	ldarg	2
	call	int32 _rem::checkfinite(float32)
	ldloc	0
	ceq
	brfalse		FAIL
	ldc.i4		0x1
	br			FINISH
COMPARE:
	ldarg	2
	ceq
	brfalse		FAIL
	ldc.i4		0x1
	br			FINISH
FAIL:
	ldc.i4	0x0001
	br			FINISH
FINISH:
	ret
}
.method public static	int32 checkfinite(float32) {
.locals init		(class [mscorlib]System.ArithmeticException)
.maxstack		3
try_start:
	ldarg	0
	ckfinite
	pop
	leave.s	try_end
try_end:
	ldc.i4		0x0
	br			FINISH
ae:
	isinst		[mscorlib]System.ArithmeticException
	stloc	0
	leave			HEnd
HEnd:
	ldloc	0
	brfalse		FAIL
	ldarg	0
	ldc.r4		float32(0xFF800000)
	ceq
	brtrue		NINF
	ldarg	0
	ldc.r4		float32(0x7F800000)
	ceq
	brtrue		PINF
	ldc.i4		0x00000001
	br			FINISH
NINF:
	ldc.i4		0x00000002
	br			FINISH
PINF:
	ldc.i4		0x00000003
	br			FINISH
NaN:
	ldc.i4		0x00000001
	br			FINISH
FAIL:
	ldc.i4		0xFF
	br			FINISH
FINISH:
	ret
.try try_start to try_end catch [mscorlib]System.ArithmeticException handler ae to HEnd
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF800000)
	ldc.r4		1.5
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0xFF7FFFFF)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0xFF7FFFFF)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		1.5
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0xBF800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0xBF800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0xBF800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0xBF800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0xBF800000)
	ldc.r4		1.5
	ldc.r4		float32(0xBF800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x80000000)
	ldc.r4		1.5
	ldc.r4		float32(0x80000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x00000000)
	ldc.r4		1.5
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x3F800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x3F800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x3F800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x3F800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3F800000)
	ldc.r4		1.5
	ldc.r4		float32(0x3F800000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x7F7FFFFF)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x7F7FFFFF)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		1.5
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7F800000)
	ldc.r4		1.5
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x7FC00000)
	ldc.r4		1.5
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0xFF800000)
	ldc.r4		float32(0x3FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0xFF7FFFFF)
	ldc.r4		float32(0x3FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0xBF800000)
	ldc.r4		float32(0x3F000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0x3F800000)
	ldc.r4		float32(0x3F000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0x7F7FFFFF)
	ldc.r4		float32(0x3FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0x7F800000)
	ldc.r4		float32(0x3FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0x7FC00000)
	ldc.r4		float32(0x7FC00000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0x3FC00000)
	ldc.r4		float32(0x00000000)
	call	int32 _rem::_rem(float32,float32,float32)
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly rem_r4exe{}
