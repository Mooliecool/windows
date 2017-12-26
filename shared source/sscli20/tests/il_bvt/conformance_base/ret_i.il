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
.method public		int32& ret_test(int32&) {
.maxstack	1
	ldarg	1
	ret
}
.method public		void _ret() {
.maxstack	0
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.locals		(class _ret,int32)
.maxstack		5
	newobj		instance void _ret::_ret()
	stloc		0
	ldc.i4		0xFF
	stloc		1
	ldloc		0
	ldloca		1
	call		instance int32& _ret::ret_test(int32&)
	ldloca		1
	ceq
	brfalse	FAIL
PASS:
	ldc.i4	0x0000
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
}
}
.assembly ret_iexe{}
