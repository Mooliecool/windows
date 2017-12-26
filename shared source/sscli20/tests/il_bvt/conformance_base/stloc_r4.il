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
.class public Stloc_r4 {
.field public static	float32 onlyhigh
.field public static	float32 none
.field public static	float32 onlylow
.method public static	void initialize() {
.maxstack	10
	ldc.r4		float32(0x80000000)
	stsfld		float32 Stloc_r4::onlyhigh
	ldc.r4		float32(0x00000000)
	stsfld		float32 Stloc_r4::none
	ldc.r4		float32(0x00000001)
	stsfld		float32 Stloc_r4::onlylow
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
.locals init	(float32,float32,float32)
	call	void Stloc_r4::initialize()
	ldc.r4		float32(0x80000000)
	ldc.r4		float32(0x00000000)
	ldc.r4		float32(0x00000001)
	stloc	2
	stloc	1
	stloc	0
	ldsfld	float32 Stloc_r4::onlyhigh
	ldloc	0
	ceq
	brfalse		FAIL
	ldsfld	float32 Stloc_r4::none
	ldloc	1
	ceq
	brfalse		FAIL
	ldsfld	float32 Stloc_r4::onlylow
	ldloc	2
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
.assembly stloc_r4exe{}
