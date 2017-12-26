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
.assembly castclassexe{}
.class public Super {
.method public void .ctor() {
.maxstack	10
	ldarg.0
	call instance void [mscorlib]System.Object::.ctor() 
	ret
}
}
.class public Sub extends Super {
.method public void .ctor() {
.maxstack	10
	ldarg.0
	call instance void Super::.ctor() 
	ret
}
}
.class public _castclass {
.method public static	int32 positivetest() {
.maxstack	10
try_start:
	newobj		instance void Sub::.ctor() 
	castclass	Super
	pop
	leave		try_end
try_end:
	ldc.i4	0x1
	ret
handler_start:
	pop
	leave	done
handler_end:
done:
	ldc.i4	0x0
	ret
.try try_start to try_end catch [mscorlib]System.InvalidCastException handler handler_start to handler_end
}
.method public static	int32 negativetest() {
.maxstack	10
try_start:
	newobj		instance void Sub::.ctor() 
	castclass	_castclass
	pop
	leave		try_end
try_end:
	ldc.i4	0x0
	ret
handler_start:
	pop
	leave	done
handler_end:
done:
	ldc.i4	0x1
	ret
.try try_start to try_end catch [mscorlib]System.InvalidCastException handler handler_start to handler_end
}
.method public static	int32 main(class [mscorlib]System.String[]) {
.entrypoint
.maxstack	10
	call		int32 _castclass::positivetest()
	brfalse		FAIL
	call		int32 _castclass::negativetest()
	brfalse		FAIL
PASS:
	ldc.i4	0x0000
	ret
FAIL:
	ldc.i4	0x0001
	ret
}
}
