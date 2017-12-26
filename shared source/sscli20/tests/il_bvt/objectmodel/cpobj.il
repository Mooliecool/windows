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
.assembly cpobjexe{}
.class public			_cpobj {
.field public static	value class valClass Instance1
.field public static	value class valClass Instance2
.method public static	int32 main() {
.entrypoint
.locals init		(class [mscorlib]System.IO.TextWriter)
.maxstack		2
	call	    class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Out()
	stloc	0
	ldsflda		value class valClass _cpobj::Instance1
	ldc.i4		0x89ABCDEF
	stfld		int32 valClass::Field
	ldc.i4		0x01234567
	stsfld		int32 valClass::SField
	ldsflda		value class valClass _cpobj::Instance1
	ldfld		int32 valClass::Field
	ldc.i4		0x89ABCDEF
	ceq
	brfalse		Init1Failure
	ldsfld		int32 valClass::SField
	ldc.i4		0x01234567
	ceq
	brtrue		InitInstance2
Init1Failure:
	ldloc	0
	ldstr		"Initialization failure in instance 1"
	callvirt	instance void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	br			fail
InitInstance2:
	ldsflda		value class valClass _cpobj::Instance2
	ldc.i4		0xABABABAB
	stfld		int32 valClass::Field
	ldsflda		value class valClass _cpobj::Instance2
	ldfld		int32 valClass::Field
	ldc.i4		0xABABABAB
	ceq
	brfalse		Init2Failure
	ldsfld		int32 valClass::SField
	ldc.i4		0x01234567
	ceq
	brtrue		CheckInst1Field
Init2Failure:
	ldloc	0
	ldstr		"Initialization failure in instance 2"
	callvirt	instance void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	br			fail
CheckInst1Field:
	ldsflda		value class valClass _cpobj::Instance1
	ldfld		int32 valClass::Field
	ldc.i4		0x89ABCDEF
	ceq
	brtrue		cpobjTests
	ldloc	0
	ldstr		"Initialization failure in instance 1"
	callvirt	instance void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	br			fail
cpobjTests:
	ldsflda		value class valClass _cpobj::Instance2
	ldsflda		value class valClass _cpobj::Instance1
	cpobj		valClass
	ldsflda		value class valClass _cpobj::Instance1
	ldfld		int32 valClass::Field
	ldc.i4		0x89ABCDEF
	ceq
	brfalse		cpobjFailure
	ldsflda		value class valClass _cpobj::Instance2
	ldfld		int32 valClass::Field
	ldc.i4		0x89ABCDEF
	ceq
	brfalse		cpobjFailure
	ldsfld		int32 valClass::SField
	ldc.i4		0x01234567
	ceq
	brtrue		pass
cpobjFailure:
	ldloc	0
	ldstr		"_cpobj failed.  inst1 fields != inst2 fields"
	callvirt	instance void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	br			fail
pass:
	ldc.i4	0x0000
	br		end
fail:
	ldc.i4	0x0001
	br		end
end:
	ret
}
}
.class public value sealed valClass {
.field public			int32 Field
.field public static	int32 SField
}
