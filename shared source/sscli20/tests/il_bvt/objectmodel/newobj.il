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
.assembly newobjexe{}
.class public	Test {
.method public void .ctor() {
.maxstack	10
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor() 
	ret
}
.method public	int32 TestMethod(int32) {
.maxstack	10
	ldarg	1
	ldc.i4		0x2
	mul
	ret
}
}
.class public _newobj {
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	newobj		instance void Test::.ctor() 
	isinst		Test
	ldnull
	ceq
	brtrue		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
