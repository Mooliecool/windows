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
.class public _ldarga {
.field public static int32 FOO
.method public static	int32 args(int32) {
.maxstack	5
	ldarga 0
	ldind.i4
	ldc.i4 0x2
	add
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		10
	ldsflda		int32 _ldarga::FOO
	ldc.i4		0x2
	add
	conv.i4
	ldsflda		int32 _ldarga::FOO
	call	int32 _ldarga::args(int32)
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
.assembly ldarga_iexe{}
