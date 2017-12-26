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
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.locals		(int32,int64,float32,float64)
.maxstack		20
	ldc.i4		0x02468ACF
	stloc		0
	ldloca		0
	mkrefany	int32
	refanyval	int32
	ldind.i4
	ldloc		0
	ceq
	brfalse		FAIL
	ldc.i8		0x1234567802468ACF
	stloc		1
	ldloca		1
	mkrefany	int64
	refanyval	int64
	ldind.i8
	ldloc		1
	ceq
	brfalse		FAIL
	ldc.r4		434641.233
	stloc		2
	ldloca		2
	mkrefany	float32
	refanyval	float32
	ldind.r4
	ldloc		2
	ceq
	brfalse		FAIL
	ldc.r8		432344641.2333241
	stloc		3
	ldloca		3
	mkrefany	float32
	refanyval	float32
	ldind.r8
	ldloc		3
	ceq
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
.assembly refsexe{}
