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
.class public		_ret {
.method public static		int64 ret_test(int64) {
.maxstack	1
	ldarg	0
	ret
}
.method public		void _ret() {
.maxstack	0
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack		9
	ldc.i8		0xAAAAAAAAAAAAAAAA
	call	int64 _ret::ret_test(int64)
	ldc.i8		0xAAAAAAAAAAAAAAAA
	ceq
	brfalse		FAIL
	ldc.i8		0x5555555555555555
	call	int64 _ret::ret_test(int64)
	ldc.i8		0x5555555555555555
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	call	int64 _ret::ret_test(int64)
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000001
	call	int64 _ret::ret_test(int64)
	ldc.i8		0x0000000000000001
	ceq
	brfalse		FAIL
	ldc.i8		0x0000000000000000
	call	int64 _ret::ret_test(int64)
	ldc.i8		0x0000000000000000
	ceq
	brfalse		FAIL
	ldc.i8		0xFFFFFFFFFFFFFFFF
	call	int64 _ret::ret_test(int64)
	ldc.i8		0xFFFFFFFFFFFFFFFF
	ceq
	brfalse		FAIL
	ldc.i8		0x8000000000000000
	call	int64 _ret::ret_test(int64)
	ldc.i8		0x8000000000000000
	ceq
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
}
}
.assembly ret_i8exe{}
