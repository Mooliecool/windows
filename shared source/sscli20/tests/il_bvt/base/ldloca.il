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
.assembly ldlocaexe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.class public _ldloca {
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
.locals	(int32,int32&)
	ldloca		0
	stloc		1
	ldc.i4		0xFF
	stloc	0
	ldloc	1
	ldind.i4
	ldc.i4		0xFF
	ceq
	brfalse		FAIL
	ldc.i4		0xAA
	stloc	0
	ldloc	1
	ldind.i4
	ldc.i4		0xAA
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
