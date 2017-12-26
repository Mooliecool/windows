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
.assembly callintfexe{}
.class public TestInterface {
.method public void .ctor(){
	.maxstack 10
	ldarg.0
	call	instance void [mscorlib]System.Object::.ctor()
	ret
}
.method public	virtual int32 TestMethod(int32) {
    .maxstack 8
    ret
}
}
.class public	TestImplementation extends TestInterface {
.method public void .ctor(){
.maxstack	10
	ldarg.0
	call instance void TestInterface::.ctor()
	ret
}
.method public	virtual int32 TestMethod(int32) {
.maxstack	10
	ldarg	1
	ldc.i4		0x2
	mul
	ret
}
}
.class public callintf {
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	newobj		instance void TestImplementation::.ctor()
	ldc.i4		0x6
	callvirt	instance int32 TestInterface::TestMethod(int32)
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
