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
.assembly ldtokenexe{}
.method public static int32 main(class System.String[]) {
.entrypoint
.locals	()
.maxstack	2
	ldtoken	method instance int32 _ldtoken::test(int32)
	pop
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
.class public	_ldtoken	 {
.field public static int32 PTR
.method public	void _ldtoken() {
.maxstack	4
	ret
}
.method public	int32 test(int32) {
.maxstack	2
	ldarg	1
	ldc.i4	0x2
	mul
	ret
}
}
