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
.class public Not_U8 {
.field public static	int64 ALL
.field public static	int64 NONE
.field public static	int64 ODD
.field public static	int64 EVEN
.field public static	int64 HIGH
.field public static	int64 LOW
.field public static	int64 RW
.method public static void initialize() {
.maxstack		20
	ldc.i8	0xFFFFFFFFFFFFFFFF
	stsfld	int64 Not_U8::ALL
	ldc.i8	0x0000000000000000
	stsfld	int64 Not_U8::NONE
	ldc.i8	0x8000000000000000
	stsfld	int64 Not_U8::HIGH
	ldc.i8	0x3AE2A8A93AE2A8A9
	stsfld	int64 Not_U8::RW
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
	call	void Not_U8::initialize()
	ldsfld	int64 Not_U8::ALL
	not
	ldc.i8	0x0000000000000000
	ceq
	brfalse	FAIL
	ldsfld	int64 Not_U8::NONE
	not
	ldc.i8	0xFFFFFFFFFFFFFFFF
	ceq
	brfalse	FAIL
	ldsfld	int64 Not_U8::HIGH
	not
	ldc.i8	0x7FFFFFFFFFFFFFFF
	ceq
	brfalse	FAIL
	ldsfld	int64 Not_U8::RW
	not
	ldc.i8	0xC51D5756C51D5756
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
.assembly not_u8exe{}
