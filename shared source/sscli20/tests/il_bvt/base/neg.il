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
.assembly negexe{}
.class public _neg {
.method public static int32 negi4(int32) {
.maxstack	2
	ldarg	0
	neg
	ret
}
.method public static int64 negi8(int64) {
.maxstack	2
	ldarg	0
	neg
	ret
}
.method public static float32 negr4(float32) {
.maxstack	2
	ldarg	0
	neg
	ret
}
.method public static float64 negr8(float64) {
.maxstack	2
	ldarg	0
	neg
	ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
        ldc.i4  0x00000002
        call    int32 _neg::negi4(int32)
        ldc.i4  0xFFFFFFFE
        ceq
        brfalse FAIL
        ldc.i8  0x0000000000000002
        call    int64 _neg::negi8(int64)
        ldc.i8  0xFFFFFFFFFFFFFFFE
        ceq
        brfalse FAIL
        ldc.r4  1.1
        call    float32 _neg::negr4(float32)
        ldc.r4  -1.1
        ceq
        brfalse FAIL
        ldc.r8  99.1
        call    float64 _neg::negr8(float64)
        ldc.r8  -99.1
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
