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
.assembly refany_testsexe{}
.method public static int32 main(class Microsoft.Runtime.String[]) {
.entrypoint
.locals		(class Microsoft.Runtime.IO.Writer,int32,int64)
.maxstack		2
	ldsfld		class Microsoft.Runtime.IO.Writer Microsoft.Runtime.Text::Out
	stloc		0
	ldc.i4		0x02468ACF
	stloc		1
	ldloca		1
	mkrefany	Microsoft.Runtime.Integer4
	ldc.i4		0x1
	ldc.i4		0x02468ACF
	ldc.i8		0x0000000000000000
	call		int32 refany_tests::Test(refany,int32,int32,int64)
	ldc.i4		0xABFAB
	ceq
	brfalse		fail
	ldc.i8		0x0123456789ABCDEF
	stloc		2
	ldloca		2
	mkrefany	Microsoft.Runtime.Integer8
	ldc.i4		0x2
	ldc.i4		0x00000000
	ldc.i8		0x0123456789ABCDEF
	call		int32 refany_tests::Test(refany,int32,int32,int64)
	ldc.i4		0xABFAB
	ceq
	brfalse		fail
pass:
	ldloc		0
	ldstr		"REFANY tests PASS"
	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldc.i4		0xABFAB
	br			end
fail:
	ldloc		0
	ldstr		"!!! FAILURE !!!  REFANY TEST(S) FAILED !!! FAILURE !!!"
	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldc.i4		0x0
	br			end
end:
	ret
}
.class public			refany_tests {
.method public static	int32 Test(refany,int32,int32,int64) {
.locals		(class Microsoft.Runtime.IO.Writer,int32,class Microsoft.Runtime.String)
.maxstack		3
	ldsfld		class Microsoft.Runtime.IO.Writer Microsoft.Runtime.Text::Out
	stloc		0
TryStart:
	ldarg		1
	ldc.i4		0x1
	ceq
	brtrue		Integer4Tests
	ldarg		1
	ldc.i4		0x2
	ceq
	brtrue		Integer8Tests
	br			InvalidArg1
Integer4Tests:
 	refanytype	0
 	callvirt	class Microsoft.Runtime.String Microsoft.Runtime.Class::GetName()
 	stloc		2
 	ldloc		0
 	ldloc		2
 	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldrefanya	0 Microsoft.Runtime.Integer4
	ldind.i4
	ldarg		2
	ceq
	brfalse		NotEqual
	typerefany	0
	callvirt	class Microsoft.Runtime.String Microsoft.Runtime.Class::GetName()
	ldstr		"Integer4"
	call		bool Microsoft.Runtime.String::Equals(class Microsoft.Runtime.String)
	brfalse		WrongType
	br			TryEnd
Integer8Tests:
	typerefany	0
	callvirt	class Microsoft.Runtime.String Microsoft.Runtime.Class::GetName()
	stloc		2
	ldloc		0
	ldloc		2
	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldrefanya	0 Microsoft.Runtime.Integer8
	ldind.i8
	ldarg		3
	ceq
	brfalse		NotEqual
	typerefany	0
	callvirt	class Microsoft.Runtime.String Microsoft.Runtime.Class::GetName()
	ldstr		"Integer8"
	call		bool Microsoft.Runtime.String::Equals(class Microsoft.Runtime.String)
	brfalse		WrongType
	br			TryEnd
TryEnd:
	ldc.i4		0xABFAB
	br			End
Filter:
	pop
	ldc.i4		0x1
	endfilter
Handler:
	callvirt	class Microsoft.Runtime.String Microsoft.Runtime.Object::ToString()
	stloc		2
	ldloc		0
	ldstr		"Looks like we got ourselves an Exception!"
	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldloc		0
	ldloc		2
	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldc.i4		0xFFFFFFFF
	br			End
InvalidArg1:
	ldloc		0
	ldstr		"One _or more arguments is invalid."
	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldc.i4		0xFFFFFFFF
	br			End
WrongType:
	ldloc		0
	ldstr		"Refany is _not of expected type."
	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldc.i4		0xFFFFFFFF
	br			End
NotEqual:
	ldloc		0
	ldstr		"Refany value is _not as expected.  (1 != 1)"
	callvirt	void Microsoft.Runtime.IO.Writer::WriteLine(class Microsoft.Runtime.String)
	ldc.i4		0xFFFFFFFF
	br			End
End:
	ret
.try TryStart to TryEnd filter Filter handler Handler
}
}
