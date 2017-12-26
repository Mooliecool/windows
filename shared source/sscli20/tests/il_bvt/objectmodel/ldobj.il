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
.assembly ldobj.exe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.class public			_ldobj {
.method public static	int32 main() {
.entrypoint
.locals		(class [mscorlib]System.IO.TextWriter,value class valClass,value class valClass)
.maxstack		2
	call	    class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Out()
	stloc	0
	newobj		void valClass::.ctor()
	stloc		1
	newobj		void valClass::.ctor()
	stloc		2
	ldloca		1
	ldc.i4		0xFAFAFAFA
	stfld		int32 valClass::Field
	ldc.i4		0xDBDBDBDB
	stsfld		int32 valClass::SField
	ldloca		2
	ldc.i4		0x00000000
	stfld		int32 valClass::Field
	ldloca		1
	ldfld		int32 valClass::Field
	ldc.i4		0xFAFAFAFA
	ceq
	brfalse		failInit
	ldsfld		int32 valClass::SField
	ldc.i4		0xDBDBDBDB
	ceq		
	brfalse		failInit
	ldloca		2
	ldfld		int32 valClass::Field
	ldc.i4		0x00000000
	ceq
	brtrue		ldlocaTest
failInit:
	ldloc	0
	ldstr		"one _or both val classes failed initialization."
	callvirt	void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	br			fail
ldlocaTest:
	ldloca		1
	ldobj		valClass
	ldloca		2
	call		void _ldobj::changeData(value class valClass,value class valClass*)
	ldloca		1
	ldfld		int32 valClass::Field
	ldc.i4		0xFAFAFAFA
	ceq
	brfalse		failTest
	ldloca		2
	ldfld		int32 valClass::Field
	ldc.i4		0xBBBBBBBB
	ceq
	brfalse		failTest
	ldsfld		int32 valClass::SField
	ldc.i4		0xDDDDDDDD
	ceq		
	brtrue		pass
failTest:
	ldloc	0
	ldstr		"failure after initialization."
	callvirt	void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	br			fail
pass:
	ldloc	0
	ldstr		"_ldobj tests PASS"
	callvirt	void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	ldc.i4		0xAAAA
	br			end
fail:
	ldloc	0
	ldstr		"!!! FAILURE !!!  LDOBJ TEST(S) FAILED !!! FAILURE !!!"
	callvirt	void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	ldc.i4		0x0
	br			end
end:
	ret
}
.method public static	void changeData(value class valClass,value class valClass*) {
.maxstack		2
	ldarga		0
	ldc.i4		0xBBBBBBBB
	stfld		int32 valClass::Field
	ldc.i4		0xDDDDDDDD
	stsfld		int32 valClass::SField
	ldarg		1
	ldarga		0
	cpobj		valClass
	ret
}
}
.class public value sealed valClass {
.field public			int32 Field
.field public static	int32 SField
	.method public void .ctor(){
		ret
	}
}
