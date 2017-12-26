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
.assembly shrexe{}
.class public _shr {
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
	ldc.i4	0x7FFFFFFF
	call	int32 _shr::return(int32)
	ldc.i4	0x1
	call	int32 _shr::return(int32)
	shr
	ldc.i4	0x3FFFFFFF
	call	int32 _shr::return(int32)
	ceq
	brfalse	FAIL
	ldc.i4	0xFFFFFFFE
	call	int32 _shr::return(int32)
	ldc.i4	0x1
	call	int32 _shr::return(int32)
	shr.un
	ldc.i4	0x7FFFFFFF
	call	int32 _shr::return(int32)
	ceq
	brfalse	FAIL
	ldc.i8	0x7FFFFFFFFFFFFFFF
	call	int64 _shr::return(int64)
	ldc.i4	0x1
	call	int32 _shr::return(int32)
	shr
	ldc.i8	0x3FFFFFFFFFFFFFFF
	call	int64 _shr::return(int64)
	ceq
	brfalse	FAIL
	ldc.i8	0xFFFFFFFFFFFFFFFE
	call	int64 _shr::return(int64)
	ldc.i4	0x1
	call	int32 _shr::return(int32)
	shr.un
	ldc.i8	0x7FFFFFFFFFFFFFFF
	call	int64 _shr::return(int64)
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
