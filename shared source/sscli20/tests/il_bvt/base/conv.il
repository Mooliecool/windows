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
.assembly convexe{}
.class public conv {
    .data TAG = int32(0xFFFFFFFF)
.method public static int32 i4(int32) {
.maxstack	2
	ldarg	0
	ret
}
.method public static int64 i8(int64) {
.maxstack	2
	ldarg	0
	ret
}
.method public static float32 r4(float32) {
.maxstack	2
	ldarg	0
	ret
}
.method public static float64 r8(float64) {
.maxstack	2
	ldarg	0
	ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
        ldc.i4  0x8F0F0F0F
        call    int32 conv::i4(int32)
        conv.i1
        ldc.i4  0x0000000F
        ceq
        brfalse FAIL
        ldc.i4  0x8F0F0F0F
        call    int32 conv::i4(int32)
        conv.i2
        ldc.i4  0x00000F0F
        ceq
        brfalse FAIL
        ldc.i4  0x8F0F0F0F
        call    int32 conv::i4(int32)
        conv.i8
        ldc.i8  0xFFFFFFFF8F0F0F0F
        ceq
        brfalse FAIL
        ldc.i8  0x8111111111111111
        call    int64 conv::i8(int64)
        conv.i4
        ldc.i4  0x11111111
        ceq
        brfalse FAIL
        ldc.i4  0xFFFFFFFF
        call    int32 conv::i4(int32)
        conv.r4
        ldc.r4  float32(0xBF800000)
        ceq
        brfalse FAIL
        ldc.r4  float32(0xBF800000)
        call    float32 conv::r4(float32)
        conv.i4
        ldc.i4  0xFFFFFFFF
        ceq
        brfalse FAIL
        ldc.i4  0xFFFFFFFF
        call    int32 conv::i4(int32)
        conv.r8
        ldc.r8  float64(0xBFF0000000000000)
        ceq
        brfalse FAIL
        ldc.r8  float64(0xBFF0000000000000)
        call    float64 conv::r8(float64)
        conv.i4
        ldc.i4  0xFFFFFFFF
        ceq
        brfalse FAIL
        ldc.i8  0xFFFFFFFFFFFFFFFF
        call    int64 conv::i8(int64)
        conv.r8
        ldc.r8  float64(0xBFF0000000000000)
        ceq
        brfalse FAIL
        ldc.r8  float64(0xBFF0000000000000)
        call    float64 conv::r8(float64)
        conv.i8
        ldc.i8  0xFFFFFFFFFFFFFFFF
        ceq
        brfalse FAIL
        ldc.r4  float32(0xBF800000)
        call    float32 conv::r4(float32)
        conv.i8
        ldc.i8  0xFFFFFFFFFFFFFFFF
        ceq
        brfalse FAIL
        ldc.r4  float32(0xBF800000)
        call    float32 conv::r4(float32)
        conv.r8
        ldc.r8  float64(0xBFF0000000000000)
        ceq
        brfalse FAIL
        ldc.r8  float64(0xBFF0000000000000)
        call    float64 conv::r8(float64)
        conv.r4
        ldc.r4  float32(0xBF800000)
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
