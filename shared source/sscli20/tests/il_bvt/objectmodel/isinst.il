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
.assembly isinstexe{}
.class public	Super {
.method public void .ctor() {
.maxstack	10
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor() 
	ret
}
.method public	int32 SuperMethod(int32) {
.maxstack	10
	ldarg	1
	ldc.i4		0x2
	mul
	ret
}
}
.class public Sub extends Super {
.method public void .ctor() {
.maxstack	10
	ldarg.0
	call instance void Super::.ctor() 
	ret
}
.method public	int32 SubMethod(int32) {
.maxstack	10
	ldarg	1
	ldc.i4		0x2
	mul
	ret
}
}
.class public _isinst {
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	newobj		instance void Sub::.ctor() 
	isinst		Sub
	brfalse		FAIL
	newobj		instance void Sub::.ctor() 
	isinst		Super	
	brfalse		FAIL
	newobj		instance void Super::.ctor() 
	isinst		Sub
	brtrue		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
