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
.assembly unalignedexe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.locals	(int32)
.maxstack	2
	ldc.i4		0x89ABCDEF
	stloc		0
	ldloca		0x0
	ldc.i4		0x1
	add
	ldc.i4		0x11
	unaligned.	0x1 
	stind.i1
	ldloca		0x0
	ldind.i4
	ldc.i4		0x89AB11EF
	bne.un		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
