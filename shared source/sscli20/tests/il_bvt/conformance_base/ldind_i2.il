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
.class public ldind_i2 {
.field public static int64 LOADME
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		20
        ldsflda		int64 ldind_i2::LOADME
        ldc.i8          0x9ABCDEF012345678
        stind.i8
        ldc.i4          0x00005678
        ldsflda		int64 ldind_i2::LOADME
        ldind.i2
        ceq
        brfalse         FAIL
        ldc.i4          0x00001234
        ldsflda		int64 ldind_i2::LOADME
	ldc.i4          0x2
        add
        ldind.i2
        ceq
        brfalse         FAIL
        ldc.i4          0xFFFFDEF0
        ldsflda		int64 ldind_i2::LOADME
        ldc.i4          0x4
        add
        ldind.i2
        ceq
        brfalse         FAIL
        ldc.i4          0xFFFF9ABC
        ldsflda		int64 ldind_i2::LOADME
        ldc.i4          0x6
        add
        ldind.i2
        ceq
        brfalse         FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
.assembly ldind_i2exe{}
