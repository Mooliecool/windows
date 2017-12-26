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
.assembly ldvirtftn.exe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.method public static int32 main(class System.String[]) {
.locals	(int32, class _ldvirtftn)
.entrypoint
.maxstack	4
	ldc.i4	0x77
	stloc		0x0
	newobj	void		_ldvirtftn::_ldvirtftn()
	stloc		0x1
	ldloc		0x1
	ldloc		0x0
	ldloc		0x1
	ldvirtftn	instance int32		_ldvirtftn::test(int32)
	calli		default	int32 (class _ldvirtftn,int32)
	ldc.i4	0xEE
	bne.un	FAIL
	newobj	void 		TestImplementation::TestImplementation()
	stloc		0x1
	ldloc		0x1
	ldloc		0x0
	ldloc		0x1
	ldvirtftn	instance int32		TestImplementation::TestMethod(int32)
	calli		default	int32 (class TestInterface,int32)
	ldc.i4	0xEE
	bne.un	FAIL
	newobj	void 		TestImplementation::TestImplementation()
	stloc		0x1
	ldloc		0x1
	ldloc		0x0
	ldloc		0x1
	ldvirtftn	instance int32		TestInterface::TestMethod(int32)
	calli		default	int32 (class TestInterface,int32)
	ldc.i4	0xEE
	bne.un	FAIL
	newobj	void 		TestImplementation::TestImplementation()
	stloc		0x1
	ldloc		0x1
	ldloc		0x0
	ldloc		0x1
	ldvirtftn	instance int32		TestInterface::TestMethod(int32)
	calli		default	int32 (class TestImplementation,int32)
	ldc.i4	0xEE
	bne.un	FAIL
PASS:
	ldc.i4	0x0000
	br		END
FAIL:
	ldc.i4	0x0001
END:
	ret
}
.class public	_ldvirtftn {
.method public	void _ldvirtftn() {
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
.class public TestInterface {
.method public virtual int32 TestMethod(int32) {
}
}
.class public TestImplementation extends TestInterface {
.method public	void TestImplementation() {
.maxstack	10
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
