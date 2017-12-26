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
.assembly jmpexe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.locals	()
.maxstack	2
	jmp		int32 _jmp::jmpTest(class [mscorlib]System.String[])
	ldc.i4	0x0001
	ret
}
.class public	_jmp	 {
.method public	void _jmp() {
.maxstack	4
	ret
}
.method public static	int32 jmpTest(class [mscorlib]System.String[]) {
.maxstack	4
	ldc.i4	0x0000
	ret
}
}
