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
.assembly ldarg_stargexe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.class public ldarg_starg {
.field public static int32 PTR
.method public void .ctor() {
.maxstack	10
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor()
	ret
}
.method public static	int32 stargi4(int32) {
.maxstack	5
	ldarg	0
	starg	0
	ldarg	0
	ret
}
.method public static	int64 stargi8(int64) {
.maxstack	5
	ldarg	0
	starg	0
	ldarg	0
	ret
}
.method public static	float32 stargr4(float32) {
.maxstack	5
	ldarg	0
	starg	0
	ldarg	0
	ret
}
.method public static	float64 stargr8(float64) {
.maxstack	5
	ldarg	0
	starg	0
	ldarg	0
	ret
}
.method public static	class ldarg_starg stargref(class ldarg_starg) {
.maxstack	5
	ldarg	0
	starg	0
	ldarg	0
	ret
}
.method public static	int32 stargi(int32*) {
.maxstack	5
	ldarg	0
	starg	0
	ldarg	0
	conv.i4
	ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
        ldc.i4          0x1
        call    int32 ldarg_starg::stargi4(int32)
        ldc.i4          0x1
        ceq
        brfalse         FAIL
        ldc.i8          0x2
        call    int64 ldarg_starg::stargi8(int64)
        ldc.i8          0x2
        ceq
        brfalse         FAIL
        ldc.r4          1.1
        call    float32 ldarg_starg::stargr4(float32)
        ldc.r4          1.1
        ceq
        brfalse         FAIL
        ldc.r8          2.2
        call    float64 ldarg_starg::stargr8(float64)
        ldc.r8          2.2
        ceq
        brfalse         FAIL
        newobj          instance void ldarg_starg::.ctor()
        dup
        call    class ldarg_starg ldarg_starg::stargref(class ldarg_starg)
        ceq
        brfalse         FAIL
        ldsflda		int32 ldarg_starg::PTR
        call    int32 ldarg_starg::stargi(int32*)
        ldsflda		int32 ldarg_starg::PTR
	conv.i4
	ceq
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
