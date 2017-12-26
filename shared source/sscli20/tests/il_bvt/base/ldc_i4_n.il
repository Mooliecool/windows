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
.class ldc_i4_n {
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack       10
        ldc.i4.0
        ldc.i4  0
        ceq
        brfalse FAIL
        ldc.i4.1
        ldc.i4  1
        ceq
        brfalse FAIL
        ldc.i4.2
        ldc.i4  2
        ceq
        brfalse FAIL
        ldc.i4.3
        ldc.i4  3
        ceq
        brfalse FAIL
        ldc.i4.4
        ldc.i4  4
        ceq
        brfalse FAIL
        ldc.i4.5
        ldc.i4  5
        ceq
        brfalse FAIL
        ldc.i4.6
        ldc.i4  6
        ceq
        brfalse FAIL
        ldc.i4.7
        ldc.i4  7
        ceq
        brfalse FAIL
        ldc.i4.8
        ldc.i4  8
        ceq
        brfalse FAIL
        ldc.i4.0
        ldc.i4.1
        sub
        ldc.i4.m1
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
.assembly ldc_i4_nexe {}
