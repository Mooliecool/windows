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
.assembly callvirtexe{}
.class public TestClass {
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
.class public _callvirt {
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	newobj		instance void TestClass::.ctor() 
	ldc.i4		0x2
	callvirt	instance int32 TestClass::TestMethod(int32)
	ldc.i4		0x4
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
