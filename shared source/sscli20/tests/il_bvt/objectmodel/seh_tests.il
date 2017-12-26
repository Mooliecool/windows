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
.assembly seh_testsexe{}
.class public		seh {
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.locals(int32)
.maxstack	5
ldc.i4	0x0
stloc	0
try_start:
	newobj instance void [mscorlib]System.Exception::.ctor()
	throw 
try_end:
aFilter:
	isinst	[mscorlib]System.Exception
	brfalse	CONTINUE
	ldc.i4  0x00000001
	br	END_FILTER
CONTINUE:
	ldc.i4  0x00000000
END_FILTER:
	endfilter
aHandler:
	isinst	[mscorlib]System.Security.SecurityException
	brfalse	aHandler_end
aHandler_end:
	ldc.i4 0xAAAA
	stloc  0
	ldstr	"Testcase successful"
	br		END
FAIL:
END:
    call void [mscorlib]System.Console::WriteLine(class [mscorlib]System.String)
	ldloc	0
	ret
.try try_start to try_end filter aFilter handler aHandler to aHandler_end
}
}
