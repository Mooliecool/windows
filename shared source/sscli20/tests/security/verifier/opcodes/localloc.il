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
.assembly extern mscorlib { }
.assembly 'localloc' { }
.class public auto ansi beforefieldinit Test extends [mscorlib]System.Object {
.method public hidebysig instance void Negative1() cil managed
{
	.maxstack  8
	ldc.i4 16
	localloc
	pop
	ret
}
.method public hidebysig instance void Negative2() cil managed
{
	.maxstack  8
	ldc.i4 16
	dup
	localloc
	pop
	call void [mscorlib]System.Console::WriteLine(int32)
	ret
}
.method public hidebysig instance void Negative3() cil managed
{
	.maxstack  8
	.try {
		ldc.i4 16
		localloc
		leave.s IL_END
	} catch [mscorlib]System.OverflowException {
		leave.s IL_END
	}
IL_END:
	ret
}
.method public hidebysig instance void Negative4() cil managed
{
	.maxstack  8
	.try {
		newobj instance void [mscorlib]System.ApplicationException::.ctor()
		throw		
	} catch [mscorlib]System.ApplicationException {
		ldc.i4 16
		localloc
		leave.s IL_END
	}
IL_END:
	ret
}
.method public hidebysig instance void Negative5() cil managed
{
	.maxstack  8
	.locals init (int32)
	.zeroinit
	.try {
	IL_TRY:
		newobj instance void [mscorlib]System.ApplicationException::.ctor()
		throw
	IL_FILTER:
		pop
		ldc.i4 16
		localloc
		pop
		ldc.i4.1
		endfilter
	IL_EXCEPT:
		leave.s IL_END
	IL_END:
		.try IL_TRY to IL_FILTER filter IL_FILTER handler IL_EXCEPT to IL_END
		leave.s IL_RET
	} catch [mscorlib]System.ApplicationException {
		leave.s IL_RET
	}
IL_RET:
	ldloc.0
	brtrue.s IL_EOM
	newobj instance void [mscorlib]System.Security.VerificationException::.ctor()
	throw
IL_EOM:
	ret
}
.method public hidebysig instance void Negative6() cil managed
{
	.maxstack  8
	ldc.i4 16
	localloc
	ldc.i4 16
	localloc
	pop
	pop
	ret
}
.method public hidebysig static int32 Main() cil managed
{
	.entrypoint
	.maxstack 8
	.locals init (class Test V_0)
	newobj     instance void Test::.ctor()
	stloc.0
	.try {
		ldloc.0
		callvirt instance void Test::Negative1()
		ldstr "Negative1() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_1
	}
IL_NEXT_1:
	.try {
		ldloc.0
		callvirt instance void Test::Negative2()
		ldstr "Negative2() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_2
	}
IL_NEXT_2:
	.try {
		ldloc.0
		callvirt instance void Test::Negative3()
		ldstr "Negative3() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_3
	}
IL_NEXT_3:
	.try {
		ldloc.0
		callvirt instance void Test::Negative4()
		ldstr "Negative4() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_4
	}
IL_NEXT_4:
	.try {
		ldloc.0
		callvirt instance void Test::Negative5()
		ldstr "Negative5() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_5
	}
IL_NEXT_5:
	.try {
		ldloc.0
		callvirt instance void Test::Negative6()
		ldstr "Negative6() passed verification."
		call void [mscorlib]System.Console::WriteLine(string)
		leave IL_FAIL
	} catch [mscorlib]System.Security.VerificationException {
		leave.s IL_NEXT_6
	}
IL_NEXT_6:
	ldc.i4 0
	ldstr "PASSED"
	br.s IL_OUT
IL_FAIL:
	ldc.i4 1
	ldstr "FAILED!"
IL_OUT:
	call void [mscorlib]System.Console::WriteLine(string)
	ret
}
.method public hidebysig specialname rtspecialname instance void .ctor() cil managed
{
	.maxstack  8
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor()
	ret
}
}
