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
    .data DATA = int32(0x00000000)
.method public static		class _ret ret_test(class _ret) {
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
.maxstack		5
	newobj		instance void _ret::_ret()
	dup
	call		class _ret _ret::ret_test(class _ret)
	ceq
	brfalse		FAIL
	newobj		instance void _ret::_ret()
	dup
	call		class _ret _ret::ret_test(class _ret)
	ceq
	brfalse		FAIL
	newobj		instance void _ret::_ret()
	dup
	call		class _ret _ret::ret_test(class _ret)
	ceq
	brfalse		FAIL
	newobj		instance void _ret::_ret()
	dup
	call		class _ret _ret::ret_test(class _ret)
	ceq
	brfalse		FAIL
	newobj		instance void _ret::_ret()
	dup
	call		class _ret _ret::ret_test(class _ret)
	ceq
	brfalse		FAIL
	newobj		instance void _ret::_ret()
	dup
	call		class _ret _ret::ret_test(class _ret)
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
.assembly ret_refexe{}
