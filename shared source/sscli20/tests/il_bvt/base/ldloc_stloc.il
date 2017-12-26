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
.assembly ldloc_stlocexe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.class public value sealed vclass{
	.field public int32 i
	.method public void .ctor() {
		.maxstack	10
		ldarg.0
		ldc.i4	0xABBA
		stfld	int32 vclass::i
		ret
	}
}
.class public ldloc_stloc {
.field public static	value class vclass	val
.field public static int32 PTR
.method public void .ctor() {
.maxstack	10
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor()
	ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
.locals	(int32,int64,float32,float64,void*,class ldloc_stloc, value class vclass)
	ldc.i4		0x7FFFFFFF
	stloc		0
	ldc.i4		0x7FFFFFFF
	ldloc		0
	ceq
	brfalse		FAIL
	ldc.i8		0x7FFFFFFFFFFFFFFF
	stloc		1
	ldc.i8		0x7FFFFFFFFFFFFFFF
	ldloc		1
	ceq
	brfalse		FAIL
	ldc.r4		float32(0xFF7FFFFF)
	stloc		2
	ldc.r4		float32(0xFF7FFFFF)
	ldloc		2
	ceq
	brfalse		FAIL
	ldc.r8		float64(0xFFEFFFFFFFFFFFFF)
	stloc		3
	ldc.r8		float64(0xFFEFFFFFFFFFFFFF)
	ldloc		3
	ceq
	brfalse		FAIL
	ldsflda		int32 ldloc_stloc::PTR
	stloc		4
	ldsflda		int32 ldloc_stloc::PTR
	conv.i4
	ldloc		4
	conv.i4
	ceq
	brfalse		FAIL
	newobj		instance void ldloc_stloc::.ctor()
	stloc		5
	ldloc		5
	isinst		ldloc_stloc
	brfalse		FAIL
	newobj		instance void vclass::.ctor()
	stsfld		value class vclass ldloc_stloc::val
	ldsfld		value class vclass ldloc_stloc::val
	stloc		6
	ldloc		6
	ldfld		int32 vclass::i
	ldc.i4		0xABBA
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
