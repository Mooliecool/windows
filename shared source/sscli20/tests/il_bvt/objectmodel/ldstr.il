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
.assembly ldstrexe{}
.class public	_ldstr {
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.locals	init (class [mscorlib]System.IO.TextWriter)
.maxstack	10
	call	    class [mscorlib]System.IO.TextWriter [mscorlib]System.Console::get_Out()
	stloc	0
	ldloc	0
	ldstr		"String 1 should equal String 2"
	callvirt	instance void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	ldloc	0
	ldstr		"This is a Test String!"
	callvirt	instance void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	ldloc	0
	ldstr		"This is a Test String!"
	callvirt	instance void [mscorlib]System.IO.TextWriter::WriteLine(class [mscorlib]System.String)
	ldstr		"This is a Test String!"
	ldstr		"This is a Test String!"
	call		instance bool [mscorlib]System.String::Equals(class [mscorlib]System.String)
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
