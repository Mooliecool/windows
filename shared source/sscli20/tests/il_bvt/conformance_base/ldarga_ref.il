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
.class public _ldarg {
.method public		void _ldarg() {
.maxstack	0
	ret
}
.method public	class _ldarg args(class _ldarg) {
.maxstack	5
	ldarga 0
	ldind.ref
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.locals(class _ldarg)
.maxstack		2
	newobj		instance void _ldarg::_ldarg()
	stloc		0
	ldloc		0
	ldloc		0
	call		instance class _ldarg _ldarg::args(class _ldarg)
	ldloc		0
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
.assembly ldarga_refexe{}
