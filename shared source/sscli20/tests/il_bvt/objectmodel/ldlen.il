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
.assembly ldlenexe{}
.class public _ldlen {
.field public static	int32[] arr
.method public static void initialize() {
.maxstack	2
	ldc.i4		0xFF
	newarr		[mscorlib]System.Int32
	stsfld		int32[] _ldlen::arr
	ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	call	void _ldlen::initialize()
	ldsfld	int32[] _ldlen::arr
	ldlen
	ldc.i4	0xFF
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
