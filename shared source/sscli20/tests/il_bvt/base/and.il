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
.assembly andexe{}
.class public _and {
.field public static   int32 even32
.field public static   int64 even64
.method public static void initialize() {
.maxstack       2
        ldc.i4  0xAAAAAAAA
        stsfld  int32 _and::even32
        ldc.i8  0xAAAAAAAAAAAAAAAA
        stsfld  int64 _and::even64
        ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	call	void _and::initialize()
	ldsfld	int32 _and::even32
	ldc.i4	0xFFFFFFFF
	and
	ldsfld	int32 _and::even32
	ceq
	brfalse	FAIL
        ldsfld  int64 _and::even64
        ldc.i8  0xFFFFFFFFFFFFFFFF
        and
        ldsfld  int64 _and::even64
        ceq
        brfalse FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
