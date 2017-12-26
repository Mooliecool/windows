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
.class public _br {
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
.locals	 init (int32,int32)
	ldc.i4		0x0
	dup
	stloc	0
	stloc	1
BEGIN:
	ldloc	0
	dup
	ldc.i4		0x0
	ceq
	ldloc	1
	add
	stloc	1
	ldc.i4		0x1
	add
	stloc	0
	br			TAG4
TAG1:
	ldloc	0
	dup
	ldc.i4		0x9
	ceq
	ldloc	1
	add
	stloc	1
	ldc.i4		0x2
	add
	stloc	0
	br			TAG3
TAG2:
	ldloc	0
	dup
	ldc.i4		0x11
	ceq
	ldloc	1
	add
	stloc	1
	ldc.i4		0x4
	add
	stloc	0
	br			END
TAG3:
	ldloc	0
	dup
	ldc.i4		0xB
	ceq
	ldloc	1
	add
	stloc	1
	ldc.i4		0x6
	add
	stloc	0
	br			TAG2
TAG4:
	ldloc	0
	dup
 	ldc.i4		0x1
	ceq
	ldloc	1
	add
	stloc	1
	ldc.i4		0x8
	add
	stloc	0
	br			TAG1
END:
	br			0
	ldloc	1
	ldc.i4		0x00000005
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
.assembly brexe{}
