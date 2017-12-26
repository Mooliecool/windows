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
.assembly callstaticexe{}
.class public TestStatic {
.method public static	int32 StaticMethodTest(int32) {
.maxstack	10
	ldarg	0
	ldc.i4		0x2
	mul
	ret
}
}
.class public callstatic {
.method public static int32 main(class System.String[]) {
.entrypoint
.maxstack	10
	ldc.i4		0x6
	call		int32 TestStatic::StaticMethodTest(int32)
	ldc.i4		0xC
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
