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
.assembly dupexe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.class public _dup {
.field public static int32 PTR
.method public static int32 _dup(int32*) {
.maxstack	2
	ldarg	0
	dup
	ceq
	ret
}
.method public static int32 _dup(int32) {
.maxstack	2
	ldarg	0
	dup
	ceq
	ret
}
.method public static int32 _dup(int64) {
.maxstack	2
	ldarg	0
	dup
	ceq
	ret	
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
        ldc.i4          0xFAFAFAFA
        call    int32 _dup::_dup(int32)
        brfalse         FAIL
        ldc.i8          0xFAFAFAFA
        call    int32 _dup::_dup(int64)
        brfalse         FAIL
        ldsflda		int32 _dup::PTR
	call	int32 _dup::_dup(int32*)
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
