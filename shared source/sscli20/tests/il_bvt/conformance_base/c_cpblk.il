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
.class          _cpblk {
.field public static int32 DATA
.field public static int32 DATA2
.method public static int32 initialize() {
        ldsflda		int32 _cpblk::DATA
        ldc.i4          0xAA
        ldc.i4          4
        initblk
        ldsflda		int32 _cpblk::DATA
        ldc.i4          4
        add
        ldc.i4          0xFF
        ldc.i4          4
        initblk
        ldsflda		int32 _cpblk::DATA
        ldind.i4
        ldc.i4          0xAAAAAAAA
        ceq
        brfalse         FAIL
        ldsflda		int32 _cpblk::DATA
        ldc.i4          4
        add
        ldind.i4
        ldc.i4          0xFFFFFFFF
        ceq
        brfalse         FAIL
        ldc.i4  0x00000001
        ret
FAIL:
	ldc.i4	0x0001
        ret
}
.method public         void _cpblk() {
.maxstack       0
        ret
}
.method public static  int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack               5
        call    int32 _cpblk::initialize()
        brfalse         FAIL
        ldsflda		int32 _cpblk::DATA
        ldc.i4          4
        add
        ldsflda		int32 _cpblk::DATA
        ldc.i4          4
        cpblk
        ldsflda		int32 _cpblk::DATA
        ldc.i4          4
        add
        ldind.i4
        ldc.i4          0xAAAAAAAA
        ceq
        brfalse         FAIL
PASS:
	ldc.i4	0x0000
        br              END
FAIL:
	ldc.i4	0x0001
END:
        ret
}
}
.assembly c_cpblk.exe {}
