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
.assembly subexe{}
.class public _sub {
.field public static native int PTR
.method public static int32 i4(int32,int32) {
.maxstack	2
	ldarg	0
	ldarg	1
	sub
	ret
}
.method public static int64 i8(int64,int64) {
.maxstack	2
	ldarg	0
	ldarg	1
	sub
	ret
}
.method public static float32 r4(float32,float32) {
.maxstack	2
	ldarg	0
	ldarg	1
	sub
	ret
}
.method public static float64 r8(float64,float64) {
.maxstack       2
        ldarg   0
        ldarg   1
        sub
        ret
}
.method public static native int i(native int,native int) {
.maxstack       2
        ldarg   0
        ldarg   1
        sub
        ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
        ldc.i4  0xFF
        ldc.i4  0xF0
        call    int32 _sub::i4(int32,int32)
        ldc.i4  0xF
        ceq
        brfalse FAIL
        ldc.i8  0xFF
        ldc.i8  0xF0
        call    int64 _sub::i8(int64,int64)
        ldc.i8  0xF
        ceq
        brfalse FAIL
        ldc.r4  5.5
        ldc.r4  1.1
        call    float32 _sub::r4(float32,float32)
        ldc.r4  4.4
        ceq
        brfalse FAIL    
        ldc.r8  5.5
        ldc.r8  1.1
        call    float64 _sub::r8(float64,float64)
        ldc.r8  4.4
        ceq
        brfalse FAIL
	ldc.i8	8
	conv.i
	stsfld	native int _sub::PTR
	ldsfld	native int _sub::PTR
	ldc.i4	0x00000001
	conv.i
	call	native int _sub::i(native int,native int)
	conv.i8
	ldc.i8	0x00000007
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
