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
.assembly notexe{}
.class public _not {
.method public static int32 return(int32) {
.maxstack	2
	ldarg	0
	ret
}
.method public static int64 return(int64) {
.maxstack	2
	ldarg	0
	ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
        ldc.i4  0xAAAAAAAA
        call    int32 _not::return(int32)
        not
        ldc.i4  0x55555555
        call    int32 _not::return(int32)
        ceq
        brfalse FAIL
        ldc.i8  0xAAAAAAAAAAAAAAAA
        call    int64 _not::return(int64)
        not
        ldc.i8  0x5555555555555555
        call    int64 _not::return(int64)
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
