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
.assembly throwexe{}
.class public	_throw {
.method public void _throw() {
.maxstack	2
	ret
}
.method public static int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
try_start:
	AEtry_start:
		newobj	instance void [mscorlib]System.ArithmeticException::.ctor() 
		throw	
	AEtry_end:
		leave		FAIL
	AEhandler_start:
		pop
		leave	done
	AEhandler_end:
	done:
		leave		PASS
try_end:
	br	FAIL
aHandler_start:
	pop
	leave	done2
aHandler_end:
done2:
	br	FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
.try AEtry_start to AEtry_end catch [mscorlib]System.ArithmeticException handler AEhandler_start to AEhandler_end
.try try_start to try_end catch [mscorlib]System.Exception handler aHandler_start to aHandler_end
}
}
