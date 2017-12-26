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
.class public _starg {
.field public static int32 FOO
.field public static int32 BAR
.method public static	int32 args(int32,int32) {
.maxstack	10
.locals	(int32*)
	ldarg		0
	ldarg		1
	ldarg		0
	ldarg		1
	starg		0
	starg		1
	ldarg		0
	ceq
	stloc		0
	ldarg		1
	ceq
	ldloc		0
	add
	ldc.i4		0x2
	ceq
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	ldsflda		int32 _starg::FOO
	ldsflda		int32 _starg::BAR
	call	int32 _starg::args(int32,int32)
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly starg_iexe{}
