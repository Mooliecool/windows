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
.assembly localloc.exe{
  .permission reqmin ['mscorlib']System.Security.Permissions.SecurityPermissionAttribute ( "SkipVerification" = true )
}
.class public			_localloc {
.method public void .ctor() {
.maxstack	10
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor() 
	ret
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.locals		(class [mscorlib]System.IO.TextWriter,int32*,int32,class [mscorlib]System.Text.StringBuilder,int32,class [mscorlib]System.String)
.entrypoint
	call		class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Out()
	stloc		0
	ldc.i4		0x0
	stloc		2
	newobj		void [mscorlib]System.Text.StringBuilder::.ctor() 
	stloc		3
TryStart:
	ldc.i4		0xFF
	localloc
	stloc		1
LOOP0:
	ldloc		2
	ldc.i4		0xFF
	bgt			END0
	ldloc		1
	ldloc		2
	add
	ldloc		2
	conv.i1
	stind.i1
	ldc.i4		0x1
	ldloc		2
	add
	stloc		2
	br			LOOP0
END0:
	ldc.i4		0x0
	stloc		2
LOOP1:
	ldloc		2
	ldc.i4		0xFF
	bgt			PassInLoop
	ldloc		1
	ldloc		2
	add
	ldind.i1
	ldloc		2
	conv.i1
	bne.un		FailInLoop
	ldc.i4		0x1
	ldloc		2
	add
	stloc		2
	br			LOOP1
END1:
	br		BeforeTryEnd
FailInLoop:
	leave		Fail
PassInLoop:
	leave		Pass
BeforeTryEnd:
TryEnd:
	br			Pass
Filter:
	pop
	ldc.i4		0x1
	endfilter
Handler:
	pop
	leave			EndHandler
EndHandler:
	br			Fail
Pass:
	ldloc			0
	ldstr			"LOCALLOC Tests PASS"
	callvirt	void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	ldc.i4		0xAAAA
	br			End
Fail:
	ldloc		0
	ldstr		"!!!! FAILURE !!!! -- LOCALLOC Tests FAIL -- !!!! FAILURE !!!!"
	callvirt	void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	ldc.i4		0x0
	br			End
End:
	ret
.try TryStart to TryEnd filter Filter handler Handler to EndHandler
}
}
