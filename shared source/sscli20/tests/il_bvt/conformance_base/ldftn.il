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
.method public static int32 foo(int32) {
    ldarg    0
    ldc.i4      0x2
    mul
    ret 
}
.method public static int32 foo(int32,int32) {
    ldarg    0
    ldarg    1
    mul
    ret 
}
.class public Ldftn {
.method public static   int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack       20
    ldftn       int32 foo(int32)
    ldftn       int32 foo(int32)
    ceq
    brfalse     FAIL
    ldftn       int32 foo(int32,int32)
    ldftn       int32 foo(int32,int32)
    ceq
    brfalse     FAIL
    ldftn       int32 foo(int32)
    ldftn       int32 foo(int32,int32)
    ceq
    brtrue      FAIL
PASS:
	ldc.i4	0x0000
    ret
FAIL:
	ldc.i4	0x0001
    ret
}
}
.assembly ldftnexe{}
