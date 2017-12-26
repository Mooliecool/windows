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
.assembly ldftn_calliexe{}
.method public static int32 test() {
	ldc.i4		0xFFAABBCC
	ret 
}
.class public ldftn_calli {
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	ldftn		int32 test()
	calli		default int32 ()
	ldc.i4		0xFFAABBCC
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
