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
.class public _add {
	.field public static int32 PTR
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	ldc.i4	0x000000FF
	ldsflda	int32 _add::PTR
	add
	conv.i4
	ldc.i4	0x000000FF
	ldsflda	int32 _add::PTR
	add
	conv.i4
	ceq
	brfalse		FAIL
	ldsflda	int32 _add::PTR
	conv.i4
	ldsflda	int32 _add::PTR
	conv.i4
	add
	ldsflda	int32 _add::PTR
	conv.i4
	ldsflda	int32 _add::PTR
	conv.i4
	add
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
.assembly add_iexe{}
